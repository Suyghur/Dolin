package com.suyghur.dolin.simple

import android.app.Application
import android.content.Context
import com.suyghur.dolin.Zege

/**
 * @author #Suyghur.
 * Created on 4/6/21
 */
class DemoApplication : Application() {

    override fun attachBaseContext(base: Context?) {
        super.attachBaseContext(base)
    }

    override fun onCreate() {
        super.onCreate()
        Zege.getDefault().initialize(this)
    }
}