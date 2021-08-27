package com.dolin.hawkeye.handler;

import android.content.Context;

import androidx.annotation.Keep;

import com.dolin.hawkeye.monitor.ActivityMonitor;
import com.dolin.hawkeye.utils.FileUtils;

import java.util.Locale;

/**
 * @author #Suyghur.
 * Created on 2021/07/30
 */
public class BoostCrashHandler {

    static {
        System.loadLibrary("dolin-hawkeye");
    }

    private long javaCrashPtr = 0L;
    private long anrCrashPtr = 0L;

    //    private static IHawkeyeRemoteCallback cxxCrashCallback = null;
    private BoostCrashHandler() {

    }

    public static BoostCrashHandler getInstance() {
        return CrashlyticsImplHolder.INSTANCE;
    }

    public void initTempFile(Context context, int capacity) {
        String javaTempFile = String.format(Locale.getDefault(), "%s/%s", FileUtils.getLogFolderDir(context), "java_log.temp");
        String anrTempFile = String.format(Locale.getDefault(), "%s/%s", FileUtils.getLogFolderDir(context), "anr_log.temp");
        this.javaCrashPtr = initTempBuffer(javaTempFile, capacity);
        this.anrCrashPtr = initTempBuffer(anrTempFile, capacity);
    }

    public boolean initNativeCrashMonitor(String socketName) {
        return initCxxCrashMonitor(socketName);
    }

    public boolean zygoteNativeCrashDaemon(String socketName, String logPath) {
//        cxxCrashCallback = callback;
        return zygoteCxxCrashDaemon(socketName, logPath);
    }

    public boolean releaseNativeCrashDaemon() {
        return releaseCxxCrashDaemon();
    }

    public void testNativeCrash() {
        testCxxCrash();
    }

    public void record(String content) {
        record(content, false);
    }

    public void record(String content, boolean isAnr) {
        if (isAnr) {
            record2Buffer(anrCrashPtr, content);
        } else {
            record2Buffer(javaCrashPtr, content);
        }
    }

    public void flush(String path) {
        flush(path, false);
    }

    public void flush(String path, boolean isAnr) {
        if (isAnr) {
            flushLog2File(anrCrashPtr, path);
        } else {
            flushLog2File(javaCrashPtr, path);
        }
    }

    public void release() {
        if (javaCrashPtr != 0L) {
            releaseBuffer(javaCrashPtr);
        }
        if (anrCrashPtr != 0L) {
            releaseBuffer(anrCrashPtr);
        }
        releaseNativeCrashDaemon();
    }

    @Keep
    private static void onCxxCrashCallback(String logPath) {
//        try {
//            if (cxxCrashCallback != null) {
//                cxxCrashCallback.onCrashCallback(logPath);
//            }
//        } catch (RemoteException e) {
//            e.printStackTrace();
//        }
//        ActivityMonitor.getInstance().finishAllActivities();
    }


    private native boolean initCxxCrashMonitor(String socketName);

    private static native boolean zygoteCxxCrashDaemon(String socketName, String logPath);

    private native boolean releaseCxxCrashDaemon();

    private native void testCxxCrash();

    private native long initTempBuffer(String bufferPath, int capacity);

    private native void record2Buffer(long bufferPtr, String content);

    private native void flushLog2File(long bufferPtr, String path);

    private native void releaseBuffer(long bufferPtr);

    private static class CrashlyticsImplHolder {
        private static final BoostCrashHandler INSTANCE = new BoostCrashHandler();
    }
}
