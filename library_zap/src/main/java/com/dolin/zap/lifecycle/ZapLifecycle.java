package com.dolin.zap.lifecycle;

import android.app.Activity;
import android.app.Application;
import android.os.Bundle;

import com.dolin.comm.impl.R4LogHandler;

/**
 * @author #Suyghur.
 * Created on 2021/05/19
 */
public class ZapLifecycle {

    private static volatile ZapLifecycle mInstance = null;

    private Application application = null;
    private R4LogHandler r4LogHandler = null;


    private final Application.ActivityLifecycleCallbacks callback = new Application.ActivityLifecycleCallbacks() {
        @Override
        public void onActivityCreated(Activity activity, Bundle savedInstanceState) {

        }

        @Override
        public void onActivityStarted(Activity activity) {

        }

        @Override
        public void onActivityResumed(Activity activity) {

        }

        @Override
        public void onActivityPaused(Activity activity) {
            if (r4LogHandler != null) {
                r4LogHandler.asyncFlush();
            }
        }

        @Override
        public void onActivityStopped(Activity activity) {

        }

        @Override
        public void onActivitySaveInstanceState(Activity activity, Bundle outState) {

        }

        @Override
        public void onActivityDestroyed(Activity activity) {

        }
    };


    private ZapLifecycle() {

    }

    public static ZapLifecycle getInstance() {
        if (mInstance == null) {
            synchronized (ZapLifecycle.class) {
                if (mInstance == null) {
                    mInstance = new ZapLifecycle();
                }
            }
        }
        return mInstance;
    }

    public void registerZapLifeCallback(Application application, R4LogHandler r4LogHandler) {
        this.application = application;
        this.r4LogHandler = r4LogHandler;
        application.registerActivityLifecycleCallbacks(callback);
    }

    public void unregisterZapLifeCallback() {
        if (application != null) {
            application.unregisterActivityLifecycleCallbacks(callback);
            this.application = null;
        }
        if (r4LogHandler != null) {
            this.r4LogHandler = null;
        }
    }
}
