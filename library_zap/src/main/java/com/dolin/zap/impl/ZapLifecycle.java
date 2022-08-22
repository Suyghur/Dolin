package com.dolin.zap.impl;

import android.app.Activity;
import android.app.Application;
import android.os.Bundle;

/**
 * @author #Suyghur.
 * Created on 2021/05/19
 */
public class ZapLifecycle {

    private static volatile ZapLifecycle mInstance = null;

    private Application application = null;


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
            ZapRecord.getInstance().asyncFlush();
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

    public void registerZapLifeCallback(Application application) {
        this.application = application;
        application.registerActivityLifecycleCallbacks(callback);
    }

    public void unregisterZapLifeCallback() {
        if (application != null) {
            application.unregisterActivityLifecycleCallbacks(callback);
            this.application = null;
        }
    }
}
