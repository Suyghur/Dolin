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
                //logcat输出最低等级
                .setLogcatLevel(Level.DEBUG)
                //是否开启缓存日志
                .setRecordEnable(true)
                //缓存日志最低等级
                .setRecordLevel(Level.DEBUG)
                //是否开启压缩缓存日志内容
                .setRecordCompressEnable(true)
                //缓存文件的过期时间
                .setOverdueDay(3)
                //缓存文件大小限制，超过则会自动扩容新文件
                .setFileSizeLimitDay(15)
                //缓存文件内容是否压缩
                .setRecordCompressEnable(false)
                .create()
        Zap.initialize(this, config)
    }
}