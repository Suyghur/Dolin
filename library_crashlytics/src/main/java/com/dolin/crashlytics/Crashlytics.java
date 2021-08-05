package com.dolin.crashlytics;

import android.app.Application;

import com.dolin.crashlytics.handler.JavaCrashHandler;
import com.dolin.crashlytics.impl.NativeBridge;
import com.dolin.crashlytics.internal.ICrashHandler;
import com.dolin.crashlytics.utils.FileUtils;

import java.io.File;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.HashMap;
import java.util.Locale;
import java.util.Map;

/**
 * @author #Suyghur.
 * Created on 2021/07/30
 */
public class Crashlytics {

    private final Map<String, String> customMap = new HashMap<>();

    private Crashlytics() {

    }

    public static Crashlytics getInstance() {
        return CrashlyticsHolder.INSTANCE;
    }

    public void initialize(Application application, ICrashHandler callback) {
        String date = new SimpleDateFormat("yyyy-MM-dd", Locale.getDefault()).format(new Date());

        String logFolderDir = FileUtils.getLogFolderDir(application);

        String bufferPath = logFolderDir + File.separator + "crashlytics.cache";
//        String logPath = FileUtils.getLogDir(logFolderDir, date);
        String logPath = logFolderDir + File.separator + "tmp.crashlytics";

//        NativeBridge.getBridge().init(bufferPath, logPath, 1024 * 400, 15 * 1024 * 1024, false);
        JavaCrashHandler.getInstance().initialize(application, customMap, callback);
    }

    public void setCustomKV(String key, int value) {
        this.customMap.put(key, String.valueOf(value));
    }

    public void setCustomKV(String key, float value) {
        this.customMap.put(key, String.valueOf(value));
    }

    public void setCustomKV(String key, double value) {
        this.customMap.put(key, String.valueOf(value));
    }


    public void setCustomKV(String key, long value) {
        this.customMap.put(key, String.valueOf(value));

    }

    public void setCustomKV(String key, String value) {
        this.customMap.put(key, value);
    }

    public void setCustomKV(String key, boolean value) {
        this.customMap.put(key, String.valueOf(value));
    }

    public void testJavaCrash(boolean runInNewThread) throws RuntimeException {
        if (runInNewThread) {
            Thread thread = new Thread() {
                @Override
                public void run() {
                    throw new RuntimeException("test java exception");
                }
            };
            thread.setName("dolin_crashlytics_test_java_thread");
            thread.start();
        } else {
            throw new RuntimeException("test java exception");
        }
    }

    private static class CrashlyticsHolder {
        private static final Crashlytics INSTANCE = new Crashlytics();
    }
}
