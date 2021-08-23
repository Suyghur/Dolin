package com.dolin.hawkeye.handler;

import android.util.Log;

import androidx.annotation.Keep;

/**
 * @author #Suyghur.
 * Created on 2021/07/30
 */
public class BoostCrashHandler {

    static {
        System.loadLibrary("dolin-hawkeye");
    }

    private long ptr = 0L;
    private String bufferPath = "";

    private BoostCrashHandler() {

    }

    public static BoostCrashHandler getInstance() {
        return CrashlyticsImplHolder.INSTANCE;
    }

    public boolean initNativeCrashMonitor(String socketName) {
        return initCxxCrashMonitor(socketName);
    }

    public boolean initNativeCrashDaemon(String socketName, String logPath) {
        return initCxxCrashDaemon(socketName, logPath);
    }

    public void testNativeCrash() {
        testCxxCrash();
    }

    public void record(String path, String content) {
        this.ptr = record2Buffer(path, content);
    }

    public void release() {
        if (ptr != 0L) {
            releaseBuffer(ptr);
        }
    }

    @Keep
    private static void onCxxCrashCallback(String logPath) {
        Log.d("dolin_hawkeye", "onCxxCrashCallback, log path: " + logPath);
    }


    private native boolean initCxxCrashMonitor(String socketName);

    private native boolean initCxxCrashDaemon(String socketName, String logPath);

    private native void testCxxCrash();

    private native long record2Buffer(String bufferPath, String content);

    private native void releaseBuffer(long bufferPtr);

    private static class CrashlyticsImplHolder {
        private static final BoostCrashHandler INSTANCE = new BoostCrashHandler();
    }
}
