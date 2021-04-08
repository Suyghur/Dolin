package com.suyghur.dolin.zap.impl

import android.app.Activity
import android.app.Application
import android.os.Bundle

/**
 * @author #Suyghur.
 * Created on 4/8/21
 */
object ZapLifecycle {

    fun registerZapLifeCallback(application: Application, record2MMap: Record2MMap) {
        application.registerActivityLifecycleCallbacks(object : Application.ActivityLifecycleCallbacks {
            override fun onActivityCreated(activity: Activity, savedInstanceState: Bundle?) {
            }

            override fun onActivityStarted(activity: Activity) {
            }

            override fun onActivityResumed(activity: Activity) {
            }

            override fun onActivityPaused(activity: Activity) {
                record2MMap.asyncFlush()
            }

            override fun onActivityStopped(activity: Activity) {
            }

            override fun onActivitySaveInstanceState(activity: Activity, outState: Bundle) {
                record2MMap.asyncFlush()
            }

            override fun onActivityDestroyed(activity: Activity) {
                record2MMap.asyncFlush()
            }

        })
    }

//    private fun initFragmentLifeCallback(): FragmentManager.FragmentLifecycleCallbacks {
//        return object : FragmentManager.FragmentLifecycleCallbacks() {
//            override fun onFragmentCreated(fm: FragmentManager, f: Fragment, savedInstanceState: Bundle?) {
//                super.onFragmentCreated(fm, f, savedInstanceState)
//            }
//
//            override fun onFragmentResumed(fm: FragmentManager, f: Fragment) {
//                super.onFragmentResumed(fm, f)
//            }
//
//        }
//    }
}