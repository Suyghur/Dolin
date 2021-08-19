package com.dolin.hawkeye.handler;

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

    public boolean initNativeCrashMonitor() {
        return initCxxCrashMonitor();
    }

    public void record(String path, String content) {
        this.ptr = record2Buffer(path, content);
    }

    public void release() {
        if (ptr != 0L) {
            releaseBuffer(ptr);
        }
    }


    private native boolean initCxxCrashMonitor();

    private native long record2Buffer(String bufferPath, String content);

    private native void releaseBuffer(long bufferPtr);

    private static class CrashlyticsImplHolder {
        private static final BoostCrashHandler INSTANCE = new BoostCrashHandler();
    }
}
