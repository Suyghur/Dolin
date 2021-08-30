package com.dolin.zap.impl;


import com.dolin.zap.internal.IRecord;
import com.dolin.zap.util.ZapFileUtils;

import java.io.File;

/**
 * @author #Suyghur.
 * Created on 2021/08/05
 */
public class ZapRecord implements IRecord {

    private long ptr = 0L;
    private String logPath = "";
    private String logDate = "";
    private String logFilePath = "";
    private int limitSize = 0;
    private int num = 1;

    static {
        System.loadLibrary("dolin-zap");
    }

    private ZapRecord() {

    }

    public static ZapRecord getInstance() {
        return ZapRecordHolder.INSTANCE;
    }

    public void init(String bufferPath, String logPath, String logDate, int capacity, int limitSize, boolean compress) {
        this.num = ZapFileUtils.getLogFileNumByDate(logPath, logDate);
        this.logPath = logPath;
        this.logDate = logDate;
        this.limitSize = limitSize;
        if (num > 1) {
            this.logFilePath = logPath + File.separator + logDate + "-p" + num + ".zap";
        } else {
            this.logFilePath = logPath + File.separator + logDate + ".zap";
        }
        try {
            this.ptr = initNative(bufferPath, logFilePath, capacity, limitSize, compress);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    @Override
    public void write(String msg) {
        if (ptr != 0L) {
            try {
                writeNative(ptr, msg);
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }

    @Override
    public void asyncFlush() {
        if (ptr != 0L) {
            try {
                //自动扩容
                if (isLogFileOverSizeNative(ptr)) {
                    this.num += 1;
                    this.logFilePath = logPath + File.separator + logDate + "-p" + num + ".zap";
                    expLogFileNative(ptr, logFilePath, limitSize);
                }
                asyncFlushNative(ptr);
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }

    @Override
    public void asyncFlush(String path) {

    }

    @Override
    public void changeLogPath(String path) {

    }

    @Override
    public void expLogFile(String path, int limitSize) {
        if (ptr != 0L) {
            try {
                expLogFileNative(ptr, path, limitSize);
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }

    @Override
    public void release() {
        try {
            if (ptr != 0L) {
                releaseNative(ptr);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }


    private native long initNative(String bufferPath, String logFilePath, int capacity, int limitSize, boolean compress);

    private native void writeNative(long ptr, String msg);

    private native void asyncFlushNative(long ptr);

    private native void expLogFileNative(long ptr, String path, int limitSize);

    private native void releaseNative(long ptr);

    private native boolean isLogFileOverSizeNative(long ptr);

    private static class ZapRecordHolder {
        private static final ZapRecord INSTANCE = new ZapRecord();
    }
}
