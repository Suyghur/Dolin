package com.dolin.crashlytics.handler;

import android.app.Application;
import android.os.Process;
import android.util.Log;

import com.dolin.comm.util.AppInfoUtils;
import com.dolin.crashlytics.internal.ICrashHandler;
import com.dolin.crashlytics.monitor.ActivityMonitor;
import com.dolin.crashlytics.utils.LogUtils;

import java.io.PrintWriter;
import java.io.StringWriter;
import java.util.Date;

/**
 * @author #Suyghur.
 * Created on 2021/07/30
 */
public class JavaCrashHandler implements Thread.UncaughtExceptionHandler {

    private ICrashHandler crashHandler = null;
    private Thread.UncaughtExceptionHandler exceptionHandler = null;
    private boolean intercept = false;
    private String processName = "";
    private String packageName = "";
    private String versionName = "";
    private final Date startTime = new Date();

    private JavaCrashHandler() {

    }

    public static JavaCrashHandler getInstance() {
        return JavaCrashHandlerHolder.INSTANCE;
    }

    public void initialize(Application application, boolean intercept, ICrashHandler crashHandler) {
        Log.d("dolin_crashlytics", "JavaCrashHandler initialize");
        this.crashHandler = crashHandler;
        this.exceptionHandler = Thread.getDefaultUncaughtExceptionHandler();
        this.processName = AppInfoUtils.getProcessName(application);
        this.packageName = application.getPackageName();
        this.versionName = AppInfoUtils.getVersionName(application);
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
        if (intercept) {
            if (exceptionHandler != null) {
                exceptionHandler.uncaughtException(thread, throwable);
            }
        } else {
            ActivityMonitor.getInstance().finishAllActivities();
            Process.killProcess(Process.myPid());
            System.exit(0);
        }
    }

    private void handleException(Thread thread, Throwable throwable) {
        Date crashTime = new Date();

        //notify the java crash
        //create log file

        //get stack info
        String stackInfo = "";
        String logcat = "";
        try {
            stackInfo = getStackInfo(crashTime, thread, throwable);
            logcat = LogUtils.getLogcat(200, 50, 50);
        } catch (Exception e) {
            e.printStackTrace();
        }

        Log.d("dolin_crashlytics", stackInfo);
        Log.d("dolin_crashlytics", logcat);
        //write info to log file

        //callback
    }

    private String getStackInfo(Date crashTime, Thread thread, Throwable throwable) {
        StringWriter sw = new StringWriter();
        PrintWriter pw = new PrintWriter(sw);
        throwable.printStackTrace(pw);
        String stackInfo = sw.toString();
        return LogUtils.getLogHeader(startTime, crashTime, "java crash", packageName, versionName) +
                LogUtils.SEP_OTHER_INFO + "\n" +
                "pid : " + Process.myPid() + " , tid : " + Process.myTid() +
                " , name : " + thread.getName() +
                " , >>> " + processName + " <<<\n" +
                "--------- beginning of crash :\n" +
                stackInfo + "\n";
    }

    private static class JavaCrashHandlerHolder {
        private static final JavaCrashHandler INSTANCE = new JavaCrashHandler();
    }
}
