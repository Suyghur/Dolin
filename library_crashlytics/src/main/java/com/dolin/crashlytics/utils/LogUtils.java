package com.dolin.crashlytics.utils;

import android.os.Build;
import android.os.Process;
import android.text.TextUtils;

import com.dolin.comm.util.DeviceInfoUtils;
import com.dolin.crashlytics.Version;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;
import java.util.Locale;

/**
 * @author #Suyghur.
 * Created on 2021/08/02
 */
public class LogUtils {

    public static final String SEP_OTHER_INFO = "--- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---";
    private static final String TIME_FORMATTER_STR = "yyyy-MM-dd HH:mm:ss.SSSZ";

    public static String getLogHeader(Date startTime, Date crashTime, String crashType, String packageName, String versionName) {
        DateFormat dateFormat = new SimpleDateFormat(TIME_FORMATTER_STR, Locale.getDefault());
        return "Dolin-Crashlytics monitor : " + Version.FULL_VERSION + "\n" +
                "Scene type : " + crashType + "\n" +
                "Start time : " + dateFormat.format(startTime) + "\n" +
                "Crash time : " + dateFormat.format(crashTime) + "\n" +
                "PackageName : " + packageName + "\n" +
                "Version : " + versionName + "\n" +
                "API : " + Build.VERSION.SDK_INT + "\n" +
                "OS : " + Build.VERSION.RELEASE + "\n" +
                "ABIs : " + DeviceInfoUtils.getCpuAbi() + "\n" +
                "Manufacturer : " + DeviceInfoUtils.getDeviceManufacturer() + "\n" +
                "Brand : " + DeviceInfoUtils.getMobileBrand() + "\n" +
                "Model : " + DeviceInfoUtils.getDeviceModel() + "\n";
    }

    public static String getLogcat(int mainLines, int systemLines, int eventLines) {
        int pid = android.os.Process.myPid();
        StringBuilder sb = new StringBuilder();

        sb.append("logcat:\n");

        if (mainLines > 0) {
            getLogcatByBufferName(pid, sb, "main", mainLines, 'D');
        }
        if (systemLines> 0) {
            getLogcatByBufferName(pid, sb, "system", systemLines, 'W');
        }
        if (eventLines > 0) {
            getLogcatByBufferName(pid, sb, "events", eventLines, 'I');
        }

        sb.append("\n");

        return sb.toString();
    }

    private static void getLogcatByBufferName(int pid, StringBuilder sb, String bufferName, int lines, char priority) {
        boolean withPid = (Build.VERSION.SDK_INT >= 24);
        String pidString = Integer.toString(pid);
        String pidLabel = " " + pidString + " ";

        //command for ProcessBuilder
        List<String> command = new ArrayList<>();
        command.add("/system/bin/logcat");
        command.add("-b");
        command.add(bufferName);
        command.add("-d");
        command.add("-v");
        command.add("threadtime");
        command.add("-t");
        command.add(Integer.toString(withPid ? lines : (int) (lines * 1.2)));
        if (withPid) {
            command.add("--pid");
            command.add(pidString);
        }
        command.add("*:" + priority);

        //append the command line
        Object[] commandArray = command.toArray();
        sb.append("--------- tail end of log ").append(bufferName);
        sb.append(" ( ").append(TextUtils.join(" ", commandArray)).append(" ) \n");

        //append logs
        BufferedReader br = null;
        String line;
        try {
            java.lang.Process process = new ProcessBuilder().command(command).start();
            br = new BufferedReader(new InputStreamReader(process.getInputStream()));
            while ((line = br.readLine()) != null) {
                if (withPid || line.contains(pidLabel)) {
                    sb.append(line).append("\n");
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            if (br != null) {
                try {
                    br.close();
                } catch (IOException ignored) {

                }
            }
        }
    }
}
