package com.dolin.crashlytics.impl;

import android.app.Application;
import android.util.Log;

import com.dolin.comm.impl.R4LogHandler;
import com.dolin.comm.internal.IRecord;
import com.dolin.crashlytics.utils.FileUtils;

import java.io.File;
import java.util.Date;

/**
 * @author #Suyghur.
 * Created on 2021/07/30
 */
public class CrashRecord implements IRecord {

    private R4LogHandler handler = null;
    public String logPath = "";
    private long ptr = 0L;

    private CrashRecord() {

    }

    public static CrashRecord getInstance() {
        return CrashRecordHolder.INSTANCE;
    }

    public void initialize(Application application, Date startTime) {
        String logFolderDir = FileUtils.getLogFolderDir(application);
        String bufferPath = logFolderDir + File.separator + "crashlytics.cache";
        //创建临时文件
//        this.logPath = String.format(Locale.getDefault(), "%s/%s_%020d_%s__%s%s", logFolderDir, "crash", startTime.getTime() * 1000, AppInfoUtils.getVersionName(application), AppInfoUtils.getProcessName(application), ".crashlytics");
        this.logPath = logFolderDir + File.separator + "crashlytics.tmp";
        Log.i("dolin_crashlytics", bufferPath);
        Log.i("dolin_crashlytics", logPath);
        this.handler = new R4LogHandler();
        this.ptr = handler.initNative(bufferPath, logPath, 1024 * 400, 15 * 1024 * 1024, false);
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
                handler.asyncFlushNative(ptr);
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }

    @Override
    public void asyncFlush(String path) {
        if (ptr != 0L && handler != null) {
            try {
                handler.asyncFlushNative2(ptr, path);
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }

    @Override
    public void changeLogPath(String path) {
//        if (ptr != 0L && handler != null) {
//            try {
//                handler.changeLogPath(ptr, path);
//            } catch (Exception e) {
//                e.printStackTrace();
//            }
//        }
    }

    @Override
    public void expLogFile(String path, int limitSize) {

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


    private static class CrashRecordHolder {
        private static final CrashRecord INSTANCE = new CrashRecord();
    }
}
