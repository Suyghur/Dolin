package com.suyghur.dolin.zap.lifecycle

import android.app.Activity
import android.app.Application
import android.os.Bundle
import com.suyghur.dolin.zap.impl.Record2MMap


/**
 * @author #Suyghur.
 * Created on 4/8/21
 */
object ZapLifecycle {

    private var record2MMap: Record2MMap? = null
    private var application: Application? = null

    private val callback = object : Application.ActivityLifecycleCallbacks {
        override fun onActivityCreated(activity: Activity, savedInstanceState: Bundle?) {
        }

        override fun onActivityStarted(activity: Activity) {
        }

        override fun onActivityResumed(activity: Activity) {
        }

        override fun onActivityPaused(activity: Activity) {
            record2MMap?.asyncFlush()
        }

        override fun onActivityStopped(activity: Activity) {
        }

        override fun onActivitySaveInstanceState(activity: Activity, outState: Bundle) {
        }

        override fun onActivityDestroyed(activity: Activity) {
        }
    }

    fun registerZapLifeCallback(application: Application, record2MMap: Record2MMap) {
        ZapLifecycle.application = application
        ZapLifecycle.record2MMap = record2MMap
        application.registerActivityLifecycleCallbacks(callback)
    }

    fun unregisterZapLifeCallback() {
        application?.apply {
            unregisterActivityLifecycleCallbacks(callback)
            application = null
        }
        record2MMap?.apply {
            record2MMap = null
        }

    }
}