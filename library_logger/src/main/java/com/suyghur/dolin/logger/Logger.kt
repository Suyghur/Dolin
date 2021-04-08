package com.suyghur.dolin.logger

import com.suyghur.dolin.logger.entity.Config
import com.suyghur.dolin.logger.impl.LoggerPrint

/**
 * @author #Suyghur.
 * Created on 4/7/21
 */
object Logger {

    fun initialize(config: Config) {
        LoggerPrint.getInstance().initialize(config)
    }

    fun d(any: Any?) {
        LoggerPrint.getInstance().d(any)
    }

    fun d(tag: String, any: Any?) {
        LoggerPrint.getInstance().d(tag, any)
    }

    fun i(any: Any?) {
        LoggerPrint.getInstance().i(any)
    }

    fun i(tag: String, any: Any?) {
        LoggerPrint.getInstance().i(tag, any)
    }

    fun w(any: Any?) {
        LoggerPrint.getInstance().w(any)
    }

    fun w(tag: String, any: Any?) {
        LoggerPrint.getInstance().w(tag, any)
    }

    fun e(any: Any?) {
        LoggerPrint.getInstance().e(any)
    }

    fun e(tag: String, any: Any?) {
        LoggerPrint.getInstance().e(tag, any)
    }
}