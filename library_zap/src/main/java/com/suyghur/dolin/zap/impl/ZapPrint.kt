package com.suyghur.dolin.zap.impl

import android.app.Application
import android.text.TextUtils
import android.util.Log
import com.suyghur.dolin.zap.entity.Config
import com.suyghur.dolin.zap.entity.Level
import com.suyghur.dolin.zap.entity.ZapData
import com.suyghur.dolin.zap.internal.IPrint
import com.suyghur.dolin.zap.util.FileUtils
import java.io.File
import java.lang.reflect.Array
import java.text.SimpleDateFormat
import java.util.*

/**
 * @author #Suyghur.
 * Created on 4/7/21
 */
class ZapPrint : IPrint {

    private var hasInitialized: Boolean = false
    private var tag = ""
    private var logDir = ""
    private var logcatLevel = Level.NONE
    private var recordLevel = Level.NONE
    private var record2MMap: Record2MMap? = null
    private var dateFileFormatter: DateFileFormatter = DateFileFormatter()


    fun initialize(application: Application, config: Config) {
        if (hasInitialized) {
            throw IllegalArgumentException("Logger already initialize")
        }

        this.logDir = if (TextUtils.isEmpty(config.logDir)) {
            FileUtils.getLogDir(application)
        } else {
            config.logDir
        }

        this.tag = if (TextUtils.isEmpty(config.tag)) {
            "dolin_zap"
        } else {
            config.tag
        }

        this.logcatLevel = config.logcatLevel
        this.recordLevel = config.recordLevel
        val logFileName = logDir + File.separator + SimpleDateFormat("yyyy-MM-dd", Locale.getDefault()).format(Date()) + "-zap"
        if (config.recordEnable) {
            record2MMap = Record2MMap("$logDir/zap.cache", 1024 * 400, logFileName, false)
            ZapLifecycle.registerZapLifeCallback(application, record2MMap!!)
        }
    }

    fun recycle() {
        record2MMap?.apply {
            ZapLifecycle.unregisterZapLifeCallback()
            asyncFlush()
            release()
        }
    }

    override fun d(any: Any?) {
        d(tag, any)
    }

    override fun d(tag: String, any: Any?) {
        print(Level.DEBUG, tag, any)
    }

    override fun i(any: Any?) {
        i(tag, any)
    }

    override fun i(tag: String, any: Any?) {
        print(Level.INFO, tag, any)
    }

    override fun w(any: Any?) {
        w(tag, any)
    }

    override fun w(tag: String, any: Any?) {
        print(Level.WARNING, tag, any)
    }

    override fun e(any: Any?) {
        e(tag, any)
    }

    override fun e(tag: String, any: Any?) {
        print(Level.ERROR, tag, any)
    }

    private fun interceptLogcat(level: Level): Boolean {
        return level.ordinal < logcatLevel.ordinal
    }

    private fun interceptRecord(level: Level): Boolean {
        return level.ordinal < recordLevel.ordinal
    }

    private fun print(level: Level, tag: String, any: Any?) {
        val msg = if (any == null) {
            "null"
        } else {
            val clz: Class<*> = any.javaClass
            if (clz.isArray) {
                val sb = StringBuilder(clz.simpleName)
                sb.append("[ ")
                for (i in 0 until Array.getLength(any)) {
                    if (i != 0) {
                        sb.append(", ")
                    }
                    val tmp = Array.get(any, i)
                    sb.append(tmp)
                }
                sb.append(" ]")
                sb.toString()
            } else {
                "$any"
            }
        }
        val data = ZapData.obtain(level, tag, msg)
        if (!interceptLogcat(level)) {
            printInner(data)
        }
        if (!interceptRecord(level)) {
            doRecord(data)
        }
        data.recycle()
    }

    private fun printInner(data: ZapData) {
        if (data.msg.length <= MAX_LENGTH_OF_SINGLE_MESSAGE) {
            doPrint(data.level, data.tag, data.msg)
            return
        }
        val msgLength = data.msg.length
        var start = 0
        var end = start + MAX_LENGTH_OF_SINGLE_MESSAGE
        while (start < msgLength) {
            doPrint(data.level, data.tag, data.msg.substring(start, end))
            start = end
            end = (start + MAX_LENGTH_OF_SINGLE_MESSAGE).coerceAtMost(msgLength)
        }
    }

    private fun doPrint(level: Level, tag: String, msg: String) {
        when (level) {
            Level.DEBUG -> Log.d(tag, msg)
            Level.INFO -> Log.i(tag, msg)
            Level.WARNING -> Log.w(tag, msg)
            Level.ERROR -> Log.e(tag, msg)
            else -> Log.i(tag, msg)
        }
    }

    private fun doRecord(data: ZapData) {
        record2MMap?.write(dateFileFormatter.format(data.level, data.tag, data.msg))
    }

    companion object {

        const val MAX_LENGTH_OF_SINGLE_MESSAGE = 4063

        fun instance(): ZapPrint {
            return LoggerPrintHolder.INSTANCE
        }

        private object LoggerPrintHolder {
            val INSTANCE = ZapPrint()
        }

        /**
         * 防止单例对象在反序列化时重新生成对象
         */
        private fun readResolve(): Any {
            return LoggerPrintHolder.INSTANCE
        }
    }

}