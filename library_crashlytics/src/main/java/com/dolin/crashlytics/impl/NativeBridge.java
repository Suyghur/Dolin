package com.dolin.crashlytics.impl;

/**
 * @author #Suyghur.
 * Created on 2021/07/30
 */
public class NativeBridge {

    static {
        System.loadLibrary("dolin-crashlytics");
    }

    private long ptr = 0L;

    private NativeBridge() {

    }


    public static NativeBridge getBridge() {
        return NativeBridgeHolder.INSTANCE;
    }

    public void init(String bufferPath, String logFilePath, int capacity, int limitSize, boolean compress) {
        this.ptr = initNative(bufferPath, logFilePath, capacity, limitSize, compress);
    }

    public void write(String msg) {
        writeNative(ptr, msg);
    }


    private native long initNative(String bufferPath, String logFilePath, int capacity, int limitSize, boolean compress);

    private native void writeNative(long ptr, String msg);

    private native void asyncFlushNative(long ptr);

    private native void releaseNative(long ptr);

    private static native void onNativeCrash();

    private static native void nativeTestCrash(int runInNewThread);


    private static class NativeBridgeHolder {
        private static final NativeBridge INSTANCE = new NativeBridge();
    }
}
