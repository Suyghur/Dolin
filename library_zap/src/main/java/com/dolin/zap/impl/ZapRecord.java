package com.dolin.zap.impl;

import com.dolin.comm.impl.R4LogHandler;
import com.dolin.comm.internal.IRecord;
import com.dolin.comm.util.LogFileUtils;

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
    private R4LogHandler handler = null;

    private ZapRecord() {

    }

    public static ZapRecord getInstance() {
        return ZapRecordHolder.INSTANCE;
    }

    public void init(String bufferPath, String logPath, String logDate, int capacity, int limitSize, boolean compress) {
        this.num = LogFileUtils.getLogFileNumByDate(logPath, logDate);
        this.logPath = logPath;
        this.logDate = logDate;
        this.limitSize = limitSize;
        if (num > 1) {
            this.logFilePath = logPath + File.separator + logDate + "-p" + num + ".zap";
        } else {
            this.logFilePath = logPath + File.separator + logDate + ".zap";
        }
        try {
            this.handler = new R4LogHandler();
            this.ptr = handler.initNative(bufferPath, logFilePath, capacity, limitSize, compress);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    @Override
    public void write(String msg) {
        if (ptr != 0L && handler != null) {
            try {
                handler.writeNative(ptr, msg);
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }

    @Override
    public void asyncFlush() {
        if (ptr != 0L && handler != null) {
            try {
                //自动扩容
                if (handler.isLogFileOverSizeNative(ptr)) {
                    this.num += 1;
                    this.logFilePath = logPath + File.separator + logDate + "-p" + num + ".zap";
                    handler.expLogFileNative(ptr, logFilePath, limitSize);
                }
                handler.asyncFlushNative(ptr);
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }

    @Override
    public void expLogFile(String path, int limitSize) {
        if (ptr != 0L && handler != null) {
            try {
                handler.expLogFileNative(ptr, path, limitSize);
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }

    @Override
    public void release() {
        if (handler != null) {
            try {
                if (ptr != 0L) {
                    handler.releaseNative(ptr);
                }
                handler = null;
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }

    private static class ZapRecordHolder {
        private static final ZapRecord INSTANCE = new ZapRecord();
    }
}
