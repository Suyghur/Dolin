package com.dolin.hawkeye.utils;

import android.os.Build;
import android.os.Debug;
import android.system.Os;
import android.text.TextUtils;

import com.dolin.comm.util.DeviceInfoUtils;
import com.dolin.hawkeye.Version;

import java.io.BufferedReader;
import java.io.File;
import java.io.FilenameFilter;
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


    private static final String MEM_INFO_FMT = "%21s %8s\n";
    private static final String MEM_INFO_FMT2 = "%21s %8s %21s %8s\n";

    public static final String SEP_OTHER_INFO = "--- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---";
    private static final String TIME_FORMATTER_STR = "yyyy-MM-dd HH:mm:ss.SSSZ";

    public static String getLogHeader(Date startTime, Date crashTime, String crashType, String packageName, String versionName) {
        DateFormat dateFormat = new SimpleDateFormat(TIME_FORMATTER_STR, Locale.getDefault());
        return "Dolin-Crashlytics monitor : " + Version.FULL_VERSION + "\n"
                + "Scene type: " + crashType + "\n"
                + "Start time: " + dateFormat.format(startTime) + "\n"
                + "Crash time: " + dateFormat.format(crashTime) + "\n"
                + "PackageName: " + packageName + "\n"
                + "Version: " + versionName + "\n"
                + "API version: " + Build.VERSION.SDK_INT + "\n"
                + "OS version: " + Build.VERSION.RELEASE + "\n"
                + "ABIs: " + DeviceInfoUtils.getCpuAbi() + "\n"
                + "Manufacturer: " + DeviceInfoUtils.getDeviceManufacturer() + "\n"
                + "Brand: " + DeviceInfoUtils.getMobileBrand() + "\n"
                + "Model: " + DeviceInfoUtils.getDeviceModel() + "\n";
    }

    public static String getLogcat(int mainLines, int systemLines, int eventLines) {
        int pid = android.os.Process.myPid();
        StringBuilder sb = new StringBuilder();

        sb.append("logcat:\n");

        if (mainLines > 0) {
            getLogcatByBufferName(pid, sb, "main", mainLines, 'D');
        }
        if (systemLines > 0) {
            getLogcatByBufferName(pid, sb, "system", systemLines, 'W');
        }
        if (eventLines > 0) {
            getLogcatByBufferName(pid, sb, "events", eventLines, 'I');
        }

        sb.append("\n");

        return sb.toString();
    }

    public static String getFds() {
        StringBuilder sb = new StringBuilder("open file:\n");
        try {
            File dir = new File("/proc/self/fd");
            File[] fds = dir.listFiles(new FilenameFilter() {
                @Override
                public boolean accept(File dir, String name) {
                    return TextUtils.isDigitsOnly(name);
                }
            });

            int count = 0;
            if (fds != null && fds.length > 0) {
                for (File fd : fds) {
                    String path = null;
                    try {
                        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
                            path = Os.readlink(fd.getAbsolutePath());
                        } else {
                            path = fd.getCanonicalPath();
                        }
                    } catch (Exception ignored) {
                    }
                    sb.append("    fd ").append(fd.getName()).append(": ").append(TextUtils.isEmpty(path) ? "???" : path.trim()).append("\n");
                    count++;
                    if (count > 1024) {
                        break;
                    }
                }
                if (fds.length > 1024) {
                    sb.append("    ......\n");
                }
                sb.append("    (number of FDs: ").append(fds.length).append(")\n");
            }
        } catch (Exception ignored) {

        }
        sb.append("\n");
        return sb.toString();
    }

    public static String getNetworkInfo() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) {
            return "network info:\nNot support on Android Q (API level 29) and later.\n";
        } else {
            return "network info:\n"
                    + " TCP over IPv4 (From: /proc/PID/net/tcp)\n" + FileUtils.getFileContent("/proc/self/net/tcp", 1024) + "-\n"
                    + " TCP over IPv6 (From: /proc/PID/net/tcp6)\n" + FileUtils.getFileContent("/proc/self/net/tcp6", 1024) + "-\n"
                    + " UDP over IPv4 (From: /proc/PID/net/udp)\n" + FileUtils.getFileContent("/proc/self/net/udp", 1024) + "-\n"
                    + " UDP over IPv6 (From: /proc/PID/net/udp6)\n" + FileUtils.getFileContent("/proc/self/net/udp6", 1024) + "-\n"
                    + " ICMP over IPv4 (From: /proc/PID/net/icmp)\n" + FileUtils.getFileContent("/proc/self/net/icmp", 256) + "-\n"
                    + " ICMP over IPv6 (From: /proc/PID/net/icmp6)\n" + FileUtils.getFileContent("/proc/self/net/icmp6", 256) + "-\n"
                    + " UNIX domain (From: /proc/PID/net/unix)\n" + FileUtils.getFileContent("/proc/self/net/unix", 256) + "\n";
        }
    }

    public static String getMemoryInfo() {
        return "memory info:\n"
                + " System Summary (From: /proc/meminfo)\n" + FileUtils.getFileContent("/proc/meminfo", 0) + "-\n"
                + " Process Status (From: /proc/PID/status)\n" + FileUtils.getFileContent("/proc/self/status", 0) + "-\n"
                + " Process Limits (From: /proc/PID/limits)\n" + FileUtils.getFileContent("/proc/self/limits", 0) + "-\n"
                + getProcessMemoryInfo() + "\n";
    }

    public static String getProcessMemoryInfo() {
        StringBuilder sb = new StringBuilder();
        sb.append(" Process Summary (From: android.os.Debug.MemoryInfo)\n");
        sb.append(String.format(Locale.getDefault(), MEM_INFO_FMT, "", "Pss(KB)"));
        sb.append(String.format(Locale.getDefault(), MEM_INFO_FMT, "", "-------"));

        try {
            Debug.MemoryInfo memoryInfo = new Debug.MemoryInfo();
            Debug.getMemoryInfo(memoryInfo);

            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
                sb.append(String.format(Locale.getDefault(), MEM_INFO_FMT, "Java Heap:", memoryInfo.getMemoryStat("summary.java-heap")));
                sb.append(String.format(Locale.getDefault(), MEM_INFO_FMT, "Native Heap:", memoryInfo.getMemoryStat("summary.native-heap")));
                sb.append(String.format(Locale.getDefault(), MEM_INFO_FMT, "Code:", memoryInfo.getMemoryStat("summary.code")));
                sb.append(String.format(Locale.getDefault(), MEM_INFO_FMT, "Stack:", memoryInfo.getMemoryStat("summary.stack")));
                sb.append(String.format(Locale.getDefault(), MEM_INFO_FMT, "Graphics:", memoryInfo.getMemoryStat("summary.graphics")));
                sb.append(String.format(Locale.getDefault(), MEM_INFO_FMT, "Private Other:", memoryInfo.getMemoryStat("summary.private-other")));
                sb.append(String.format(Locale.getDefault(), MEM_INFO_FMT, "System:", memoryInfo.getMemoryStat("summary.system")));
                sb.append(String.format(Locale.getDefault(), MEM_INFO_FMT2, "TOTAL:", memoryInfo.getMemoryStat("summary.total-pss"), "TOTAL SWAP:", memoryInfo.getMemoryStat("summary.total-swap")));
            } else {
                sb.append(String.format(Locale.getDefault(), MEM_INFO_FMT, "Java Heap:", "~ " + memoryInfo.dalvikPrivateDirty));
                sb.append(String.format(Locale.getDefault(), MEM_INFO_FMT, "Native Heap:", memoryInfo.nativePrivateDirty));
                sb.append(String.format(Locale.getDefault(), MEM_INFO_FMT, "Private Other:", "~ " + memoryInfo.otherPrivateDirty));
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {
                    sb.append(String.format(Locale.getDefault(), MEM_INFO_FMT, "System:", (memoryInfo.getTotalPss() - memoryInfo.getTotalPrivateDirty() - memoryInfo.getTotalPrivateClean())));
                } else {
                    sb.append(String.format(Locale.getDefault(), MEM_INFO_FMT, "System:", "~ " + (memoryInfo.getTotalPss() - memoryInfo.getTotalPrivateDirty())));
                }
                sb.append(String.format(Locale.getDefault(), MEM_INFO_FMT, "TOTAL:", memoryInfo.getTotalPss()));
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return sb.toString();
    }

    private static void getLogcatByBufferName(int pid, StringBuilder sb, String bufferName, int lines, char priority) {
        boolean withPid = (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N);
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
        sb.append(" (").append(TextUtils.join(" ", commandArray)).append(")\n");

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
