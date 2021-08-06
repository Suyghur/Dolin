package com.dolin.crashlytics;

import android.app.Application;

import com.dolin.crashlytics.handler.JavaCrashHandler;
import com.dolin.crashlytics.impl.CrashRecord;
import com.dolin.crashlytics.internal.ICrashHandler;
import com.dolin.crashlytics.monitor.ActivityMonitor;

import java.util.Date;
import java.util.HashMap;
import java.util.Map;

/**
 * @author #Suyghur.
 * Created on 2021/07/30
 */
public class Crashlytics {

    private final Map<String, String> customMap = new HashMap<>();

    private final Date startTime = new Date();

    private Crashlytics() {

    }

    public static Crashlytics getInstance() {
        return CrashlyticsHolder.INSTANCE;
    }

    public void initialize(Application application, ICrashHandler callback) {
        ActivityMonitor.getInstance().initialize(application);
        CrashRecord.getInstance().initialize(application, startTime);
        JavaCrashHandler.getInstance().initialize(application, customMap, startTime, callback);
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
