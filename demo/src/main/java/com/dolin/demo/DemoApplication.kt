package com.dolin.demo

import android.app.Application
import android.content.Context
import android.util.Log
import com.dolin.comm.util.AppInfoUtils
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
        Hawkeye.getInstance().initialize(this) { logPath, stackInfo ->
            //TODO 发生会崩时回调，进行网络请求，上报崩溃日志
        }
    }

    override fun onCreate() {
        super.onCreate()
        val config = Config.Builder()
            //logcat输出最低等级
            .setLogcatLevel(Level.DEBUG)
            //是否开启缓存日志
            .setRecordEnable(true)
            //缓存日志最低等级
            .setRecordLevel(Level.DEBUG)
            //是否开启压缩缓存日志内容
            .setCompressEnable(false)
            //缓存文件的过期时间
            .setOverdueDay(3)
            //缓存文件大小限制，超过则会自动扩容新文件
            .setFileSizeLimitDay(15)
            .create()
        Zap.initialize(this, config)
    }
}