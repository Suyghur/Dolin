package com.dolin.zap.lifecycle;

import android.app.Activity;
import android.app.Application;
import android.os.Bundle;

import com.dolin.zap.impl.Record2MMap;

/**
 * @author #Suyghur.
 * Created on 2021/05/19
 */
public class ZapLifecycle {

    private static volatile ZapLifecycle mInstance = null;

    private Application application = null;
    private Record2MMap record2MMap = null;


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
            if (record2MMap != null) {
                record2MMap.asyncFlush();
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

    public void registerZapLifeCallback(Application application, Record2MMap record2MMap) {
        this.application = application;
        this.record2MMap = record2MMap;
        application.registerActivityLifecycleCallbacks(callback);
    }

    public void unregisterZapLifeCallback() {
        if (application != null) {
            application.unregisterActivityLifecycleCallbacks(callback);
            this.application = null;
        }
        if (record2MMap != null) {
            this.record2MMap = null;
        }
    }
}
