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

    @JvmStatic
    fun initialize(application: Application, config: Config) {
        sZapPrint = ZapPrint.instance()
        sZapPrint.initialize(application, config)
        hasInitialized = true
    }

    @JvmStatic
    fun recycle() {
        if (hasInitialized)
            sZapPrint.recycle()
    }

    @JvmStatic
    fun d(any: Any?) {
        if (hasInitialized)
            sZapPrint.d(any)
    }

    @JvmStatic
    fun d(tag: String, any: Any?) {
        if (hasInitialized)
            sZapPrint.d(tag, any)
    }

    @JvmStatic
    fun i(any: Any?) {
        if (hasInitialized)
            sZapPrint.i(any)
    }

    @JvmStatic
    fun i(tag: String, any: Any?) {
        if (hasInitialized)
            sZapPrint.i(tag, any)
    }

    @JvmStatic
    fun w(any: Any?) {
        if (hasInitialized)
            sZapPrint.w(any)
    }

    @JvmStatic
    fun w(tag: String, any: Any?) {
        if (hasInitialized)
            sZapPrint.w(tag, any)
    }

    @JvmStatic
    fun e(any: Any?) {
        if (hasInitialized)
            sZapPrint.e(any)
    }

    @JvmStatic
    fun e(tag: String, any: Any?) {
        if (hasInitialized)
            sZapPrint.e(tag, any)
    }

    @JvmStatic
    fun getVersion(): String {
        return Version.VERSION
    }

    @JvmStatic
    fun getFullVersion(): String {
        return Version.FULL_VERSION
    }
}