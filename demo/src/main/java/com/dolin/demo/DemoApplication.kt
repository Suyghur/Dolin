package com.dolin.demo

import android.app.Application
import android.content.Context
import com.dolin.hawkeye.Hawkeye
import com.dolin.zap.Zap
import com.dolin.zap.entity.Config
import com.dolin.zap.entity.Level

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
        Hawkeye.getInstance().initialize(this) { logPath, stackInfo ->
            //TODO 发生会崩时回调，进行网络请求，上报崩溃日志
        }
    }
}