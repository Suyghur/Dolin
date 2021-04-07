package com.suyghur.dolin.logger.impl

import androidx.annotation.Keep
import com.suyghur.dolin.logger.Config
import com.suyghur.dolin.logger.internal.ILogger

/**
 * @author #Suyghur.
 * Created on 4/7/21
 */
class LoggerImpl : ILogger {

    private var hasInitialized: Boolean = false
    private var config: Config? = null

    fun initialize(config: Config) {
        if (hasInitialized) {
            throw IllegalArgumentException("Logger already initialize")
        }
        this.config = config

    }

    override fun v(tag: String, msg: String, vararg args: Any) {
        TODO("Not yet implemented")
    }

    override fun d(tag: String, msg: String, vararg args: Any) {
        TODO("Not yet implemented")
    }

    override fun d(tag: String, any: Any) {
        TODO("Not yet implemented")
    }

    override fun i(tag: String, msg: String, vararg args: Any) {
        TODO("Not yet implemented")
    }

    override fun w(tag: String, msg: String, vararg args: Any) {
        TODO("Not yet implemented")
    }

    override fun e(tag: String, msg: String, vararg args: Any) {
        TODO("Not yet implemented")
    }

    override fun e(throwable: Throwable, tag: String, msg: String, vararg args: Any) {
        TODO("Not yet implemented")
    }

    override fun record(level: Int, tag: String, msg: String) {
        TODO("Not yet implemented")
    }

    override fun print(tag: String, msg: String, vararg args: Any) {
        TODO("Not yet implemented")
    }

    override fun print(tag: String, any: Any) {
        TODO("Not yet implemented")
    }

    override fun print(level: Int, tag: String, msg: String) {
        TODO("Not yet implemented")
    }

    companion object {

        @Keep
        @JvmStatic
        fun getInstance(): LoggerImpl {
            return LoggerImplHolder.INSTANCE
        }

        private object LoggerImplHolder {
            val INSTANCE = LoggerImpl()
        }
    }


}