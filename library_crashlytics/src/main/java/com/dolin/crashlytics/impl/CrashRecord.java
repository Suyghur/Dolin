package com.dolin.crashlytics.impl;

import android.app.Application;

import com.dolin.comm.impl.R4LogHandler;
import com.dolin.crashlytics.utils.FileUtils;

import java.io.File;

/**
 * @author #Suyghur.
 * Created on 2021/07/30
 */
public class CrashRecord {

    private R4LogHandler handler=null;

    private CrashRecord() {

    }

    private CrashRecord getInstance() {
        return CrashRecordHolder.INSTANCE;
    }

    public void init(Application application) {
        String logFolderDir = FileUtils.getLogFolderDir(application);

        String bufferPath = logFolderDir + File.separator + "crashlytics.cache";
//        String logPath = FileUtils.getLogDir(logFolderDir, date);
        String logPath = logFolderDir + File.separator + "tmp.crashlytics";
//        handler=R4LogHandler(logFolderDir)
    }


    private static class CrashRecordHolder {
        private static final CrashRecord INSTANCE = new CrashRecord();
    }
}
