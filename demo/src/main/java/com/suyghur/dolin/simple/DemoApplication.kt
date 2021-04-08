package com.suyghur.dolin.simple

import android.app.Application
import android.content.Context
import com.suyghur.dolin.logger.entity.Config
import com.suyghur.dolin.logger.entity.Level
import com.suyghur.dolin.logger.Logger

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
        val config = Config.Builder(this)
                .setLogcatLevel(Level.DEBUG)
                .setRecordEnable(true)
                .setRecordLevel(Level.DEBUG)
                .setOverdueDay(3)
                .create()
        Logger.initialize(config)
    }
}