package com.dolin.zap

import android.app.Application
import com.dolin.zap.entity.Config
import com.dolin.zap.impl.ZapPrint

/**
 * @author #Suyghur.
 * Created on 4/7/21
 */
object Zap {

    private lateinit var sZapPrint: ZapPrint
    private var hasInitialized = false

    fun initialize(application: Application, config: Config) {
        sZapPrint = ZapPrint.instance()
        sZapPrint.initialize(application, config)
        hasInitialized = true
    }

    fun recycle() {
        if (hasInitialized)
            sZapPrint.recycle()
    }

    fun d(any: Any?) {
        if (hasInitialized)
            sZapPrint.d(any)
    }

    fun d(tag: String, any: Any?) {
        if (hasInitialized)
            sZapPrint.d(tag, any)
    }

    fun i(any: Any?) {
        if (hasInitialized)
            sZapPrint.i(any)
    }

    fun i(tag: String, any: Any?) {
        if (hasInitialized)
            sZapPrint.i(tag, any)
    }

    fun w(any: Any?) {
        if (hasInitialized)
            sZapPrint.w(any)
    }

    fun w(tag: String, any: Any?) {
        if (hasInitialized)
            sZapPrint.w(tag, any)
    }

    fun e(any: Any?) {
        if (hasInitialized)
            sZapPrint.e(any)
    }

    fun e(tag: String, any: Any?) {
        if (hasInitialized)
            sZapPrint.e(tag, any)
    }
}