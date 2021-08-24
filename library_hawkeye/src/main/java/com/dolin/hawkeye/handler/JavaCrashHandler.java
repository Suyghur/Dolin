package com.dolin.hawkeye.handler;

import android.app.Application;
import android.os.Process;
import android.util.Log;

import com.dolin.comm.util.AppInfoUtils;
import com.dolin.hawkeye.internal.ICrashHandler;
import com.dolin.hawkeye.monitor.ActivityMonitor;
import com.dolin.hawkeye.utils.FileUtils;
import com.dolin.hawkeye.utils.LogUtils;

import java.io.PrintWriter;
import java.io.StringWriter;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;
import java.util.Locale;
import java.util.Map;
import java.util.regex.Pattern;

/**
 * @author #Suyghur.
 * Created on 2021/07/30
 */
public class JavaCrashHandler implements Thread.UncaughtExceptionHandler {
    private final Date startTime = new Date();

    private ICrashHandler crashHandler = null;
    private Thread.UncaughtExceptionHandler exceptionHandler = null;
    private boolean intercept = false;
    private String processName = "";
    private String packageName = "";
    private String versionName = "";
    private String logFolderDir = "";
    private Map<String, String> customMap = null;
    private final List<Pattern> dumpAllThreadsWhiteList = new ArrayList<Pattern>() {
        {
            add(Pattern.compile("^main$"));
            add(Pattern.compile("^Binder:.*"));
            add(Pattern.compile(".*Finalizer.*"));
        }
    };

    private JavaCrashHandler() {

    }

    public static JavaCrashHandler getInstance() {
        return JavaCrashHandlerHolder.INSTANCE;
    }

    public void initialize(Application application, Map<String, String> customMap, ICrashHandler crashHandler) {
        this.crashHandler = crashHandler;
        this.customMap = customMap;
        this.exceptionHandler = Thread.getDefaultUncaughtExceptionHandler();
        this.processName = AppInfoUtils.getProcessName(application);
        this.packageName = application.getPackageName();
        this.versionName = AppInfoUtils.getVersionName(application);
        this.logFolderDir = FileUtils.getLogFolderDir(application);
        try {
            Thread.setDefaultUncaughtExceptionHandler(this);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    @Override
    public void uncaughtException(Thread thread, Throwable throwable) {
        if (exceptionHandler != null) {
            Thread.setDefaultUncaughtExceptionHandler(exceptionHandler);
        }
        try {
            handleException(thread, throwable);
        } catch (Exception e) {
            e.printStackTrace();
        }
//        CrashRecord.getInstance().release();
        if (intercept) {
            if (exceptionHandler != null) {
                exceptionHandler.uncaughtException(thread, throwable);
            }
        } else {
            Log.d("dolin_hawkeye", "11111");
            ActivityMonitor.getInstance().finishAllActivities();
            Process.killProcess(Process.myPid());
            System.exit(0);
        }
    }

    private void handleException(Thread thread, Throwable throwable) {
        Date crashTime = new Date();
        //notify the java crash

        //get stack info
        StringBuilder sb = new StringBuilder();
        try {
            sb.append(getStackInfo(crashTime, thread, throwable)).append("\n");
            sb.append(LogUtils.SEP_OTHER_INFO).append("\n");
            sb.append(LogUtils.getLogcat(200, 50, 50)).append("\n");
            sb.append(LogUtils.SEP_OTHER_INFO).append("\n");
            sb.append(LogUtils.getFds()).append("\n");
            sb.append(LogUtils.SEP_OTHER_INFO).append("\n");
            sb.append(LogUtils.getNetworkInfo()).append("\n");
            sb.append(LogUtils.SEP_OTHER_INFO).append("\n");
            sb.append(LogUtils.getMemoryInfo()).append("\n");
            sb.append(getOtherThreadsInfo(thread)).append("\n");
        } catch (Exception e) {
            e.printStackTrace();
        }

        //write info to log file
        String logPath = String.format(Locale.getDefault(), "%s/%s_%020d%s", logFolderDir, "java-crash", crashTime.getTime() * 1000, ".crash");
        BoostCrashHandler.getInstance().record(sb.toString());
//        CrashRecord.getInstance().asyncFlush();

//        CrashRecord.getInstance().changeLogPath(logPath);
//        CrashRecord.getInstance().asyncFlush(this.logPath);


        //callback
        crashHandler.onCrash("", "");
    }

    private String getStackInfo(Date crashTime, Thread thread, Throwable throwable) {
        StringWriter sw = new StringWriter();
        PrintWriter pw = new PrintWriter(sw);
        throwable.printStackTrace(pw);
        String stackInfo = sw.toString();
        boolean isForeground = ActivityMonitor.getInstance().isApplicationForeground();
        return LogUtils.getLogHeader(startTime, crashTime, "java", packageName, versionName, isForeground)
                + LogUtils.SEP_OTHER_INFO + "\n"
                + getCustomKVInfo(customMap) + "\n"
                + LogUtils.SEP_OTHER_INFO + "\n"
                + "pid: " + Process.myPid()
                + ", tid: " + Process.myTid()
                + ", name: " + thread.getName()
                + ", >>> " + processName + " <<<\n"
                + "java stacktrace:\n"
                + stackInfo + "\n";
    }

    private String getCustomKVInfo(Map<String, String> customMap) {
        if (customMap == null || customMap.size() <= 0) {
            return "Custom key-value:\n";
        } else {
            StringBuilder sb = new StringBuilder("Custom key-value:\n");
            for (Map.Entry<String, String> entry : customMap.entrySet()) {
                String key = entry.getKey();
                String value = entry.getValue();
                sb.append(key).append(": ").append(value).append("\n");
            }
            return sb.toString();
        }
    }

    private String getOtherThreadsInfo(Thread crashedThread) {
        int thdMatchedRegex = 0;
        int thdIgnoredByLimit = 0;
        int thdDumped = 0;
        StringBuilder sb = new StringBuilder();
        Map<Thread, StackTraceElement[]> map = Thread.getAllStackTraces();
        for (Map.Entry<Thread, StackTraceElement[]> entry : map.entrySet()) {
            Thread thread = entry.getKey();
            StackTraceElement[] stackTraceElements = entry.getValue();

            //skip the crashed thread
            if (thread.getName().equals(crashedThread.getName())) {
                continue;
            }

            //check regex for thread name
            if (!matchThreadName(dumpAllThreadsWhiteList, thread.getName())) {
                continue;
            }
            thdMatchedRegex++;

            //check dump count limit
            if (thdDumped >= 10) {
                thdIgnoredByLimit++;
                continue;
            }

            sb.append(LogUtils.SEP_OTHER_INFO + "\n");
            sb.append("pid: ").append(Process.myPid()).append(", tid: ").append(thread.getId()).append(", name: ").append(thread.getName()).append(" >>> ").append(processName).append(" <<<\n");
            sb.append("\n");
            sb.append("java stacktrace:\n");
            for (StackTraceElement element : stackTraceElements) {
                sb.append("    at ").append(element.toString()).append("\n");
            }
            sb.append("\n");
            thdDumped++;
        }

        if (map.size() > 1) {
            if (thdDumped == 0) {
                sb.append(LogUtils.SEP_OTHER_INFO + "\n");
            }

            sb.append("total JVM threads (exclude the crashed thread): ").append(map.size() - 1).append("\n");
            sb.append("JVM threads matched white list: ").append(thdMatchedRegex).append("\n");
            sb.append("JVM threads ignored by max count limit: ").append(thdIgnoredByLimit).append("\n");
            sb.append("dumped JVM threads: ").append(thdDumped).append("\n");
            sb.append(LogUtils.SEP_OTHER_INFO).append("\n");
        }
        return sb.toString();
    }

    private boolean matchThreadName(List<Pattern> whiteList, String threadName) {
        for (Pattern pattern : whiteList) {
            if (pattern.matcher(threadName).matches()) {
                return true;
            }
        }
        return false;
    }

    private static class JavaCrashHandlerHolder {
        private static final JavaCrashHandler INSTANCE = new JavaCrashHandler();
    }
}
