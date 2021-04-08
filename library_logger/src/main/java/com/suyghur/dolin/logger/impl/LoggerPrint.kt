package com.suyghur.dolin.logger.impl

import android.util.Log
import androidx.annotation.Keep
import com.suyghur.dolin.logger.entity.Config
import com.suyghur.dolin.logger.entity.Level
import com.suyghur.dolin.logger.entity.LoggerData
import com.suyghur.dolin.logger.internal.ILogger
import java.lang.reflect.Array

/**
 * @author #Suyghur.
 * Created on 4/7/21
 */
class LoggerPrint : ILogger {

    private var hasInitialized: Boolean = false
    private var tag = ""
    private var logcatLevel = Level.NONE
    private var recordLevel = Level.NONE
    private var record2MMap: Record2MMap? = null

    fun initialize(config: Config) {
        if (hasInitialized) {
            throw IllegalArgumentException("Logger already initialize")
        }
        this.tag = config.defaultTag
        this.logcatLevel = config.logcatLevel
        this.recordLevel = config.recordLevel
        if (config.recordEnable) {
//            record2MMap = Record2MMap(config.logDir)
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

    override fun record(level: Int, tag: String, msg: String) {
        TODO("Not yet implemented")
    }

    private fun interceptLogcat(level: Level): Boolean {
        return level.ordinal < logcatLevel.ordinal
    }

    private fun interceptRecord(level: Level): Boolean {
        return level.ordinal >= recordLevel.ordinal
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
        val data = LoggerData.obtain(level, tag, msg)
        if (!interceptLogcat(level)) {
            printInner(data)
        }
        if (!interceptRecord(level)) {
            recordLevel
        }
        data.recycle()
    }

    private fun printInner(data: LoggerData) {
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

    companion object {

        const val MAX_LENGTH_OF_SINGLE_MESSAGE = 4063

        @Keep
        @JvmStatic
        fun getInstance(): LoggerPrint {
            return LoggerPrintHolder.INSTANCE
        }

        private object LoggerPrintHolder {
            val INSTANCE = LoggerPrint()
        }

        /**
         * 防止单例对象在反序列化时重新生成对象
         */
        private fun readResolve(): Any {
            return LoggerPrintHolder.INSTANCE
        }
    }


}