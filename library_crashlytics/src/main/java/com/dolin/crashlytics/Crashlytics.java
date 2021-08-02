package com.dolin.crashlytics;

import android.app.Application;
import android.util.Log;

import com.dolin.crashlytics.handler.JavaCrashHandler;

/**
 * @author #Suyghur.
 * Created on 2021/07/30
 */
public class Crashlytics {

    private Crashlytics() {

    }

    public static Crashlytics getInstance() {
        return CrashlyticsHolder.INSTANCE;
    }

    public void initialize(Application application) {
        JavaCrashHandler.getInstance().initialize(application,false,null);
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
