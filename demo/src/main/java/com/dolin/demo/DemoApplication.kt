package com.dolin.demo

import android.app.Application
import android.content.Context
import com.dolin.zap.entity.Config
import com.dolin.zap.entity.Level
import com.dolin.zap.Zap

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
        val config = Config.Builder()
                .setLogcatLevel(Level.DEBUG)
                .setRecordEnable(true)
                .setRecordLevel(Level.DEBUG)
                .setOverdueDay(3)
                .create()
        Zap.initialize(this, config)
    }
}