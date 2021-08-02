package com.dolin.crashlytics.monitor;

import android.app.Activity;
import android.app.Application;
import android.os.Bundle;

import java.util.LinkedList;

/**
 * @author #Suyghur.
 * Created on 2021/07/30
 */
public class ActivityMonitor {

    private static final int MAX_ACTIVITY_NUM = 100;

    private LinkedList<Activity> activities = null;
    private boolean isAppForeground = false;


    private ActivityMonitor() {

    }

    public static ActivityMonitor getInstance() {
        return ActivityMonitorHolder.INSTANCE;
    }

    public void initialize(Application application) {
        if (activities == null) {
            activities = new LinkedList<>();
        }

        if (activities.size() > 0) {
            activities.clear();
        }

        application.registerActivityLifecycleCallbacks(new Application.ActivityLifecycleCallbacks() {

            private int activityReferences = 0;
            private boolean isActivityChangingConfigurations = false;

            @Override
            public void onActivityCreated(Activity activity, Bundle savedInstanceState) {
                activities.addFirst(activity);
                if (activities.size() > MAX_ACTIVITY_NUM) {
                    activities.removeLast();
                }
            }

            @Override
            public void onActivityStarted(Activity activity) {
                if (++activityReferences == 1 && !isActivityChangingConfigurations) {
                    isAppForeground = true;
                }
            }

            @Override
            public void onActivityResumed(Activity activity) {

            }

            @Override
            public void onActivityPaused(Activity activity) {

            }

            @Override
            public void onActivityStopped(Activity activity) {
                isActivityChangingConfigurations = activity.isChangingConfigurations();
                if (--activityReferences == 0 && !isActivityChangingConfigurations) {
                    isAppForeground = false;
                }
            }

            @Override
            public void onActivitySaveInstanceState(Activity activity, Bundle outState) {

            }

            @Override
            public void onActivityDestroyed(Activity activity) {

            }
        });
    }

    public void finishAllActivities() {
        if (activities != null) {
            for (Activity activity : activities) {
                activity.finish();
            }
            activities.clear();
        }
    }

    public boolean isApplicationForeground() {
        return this.isAppForeground;
    }

    private static class ActivityMonitorHolder {
        private static final ActivityMonitor INSTANCE = new ActivityMonitor();
    }
}
