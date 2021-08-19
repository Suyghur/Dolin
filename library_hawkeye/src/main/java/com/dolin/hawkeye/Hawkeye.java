package com.dolin.hawkeye;

import android.app.Application;

import com.dolin.comm.util.AppInfoUtils;
import com.dolin.hawkeye.handler.JavaCrashHandler;
import com.dolin.hawkeye.handler.NativeCrashHandler;
import com.dolin.hawkeye.internal.ICrashHandler;
import com.dolin.hawkeye.monitor.ActivityMonitor;

import org.json.JSONObject;

import java.util.Date;
import java.util.HashMap;
import java.util.Map;

/**
 * @author #Suyghur.
 * Created on 2021/07/30
 */
public class Hawkeye {

    private final Map<String, String> customMap = new HashMap<>();

    private final Date startTime = new Date();

    private Hawkeye() {

    }

    public static Hawkeye getInstance() {
        return HawkeyeHolder.INSTANCE;
    }

    public void initialize(Application application, ICrashHandler callback) {
        if (!AppInfoUtils.getProcessName(application).equals(application.getPackageName())) {
            return;
        }
        ActivityMonitor.getInstance().initialize(application);
        JavaCrashHandler.getInstance().initialize(application, customMap, callback);
        NativeCrashHandler.getInstance().initialize(application, customMap, callback);
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
            thread.setName("dolin_hawkeye_test_java_thread");
            thread.start();
        } else {
            throw new RuntimeException("test java exception");
        }
    }

    private static class HawkeyeHolder {
        private static final Hawkeye INSTANCE = new Hawkeye();
    }
}
