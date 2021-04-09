package com.suyghur.dolin.zap

import android.app.Application
import com.suyghur.dolin.zap.entity.Config
import com.suyghur.dolin.zap.impl.ZapPrintImpl

/**
 * @author #Suyghur.
 * Created on 4/7/21
 */
object Zap {

    fun initialize(application: Application, config: Config) {
        ZapPrintImpl.getInstance().initialize(application, config)
    }

    fun recycle() {
        ZapPrintImpl.getInstance().recycle()
    }

    fun d(any: Any?) {
        ZapPrintImpl.getInstance().d(any)
    }

    fun d(tag: String, any: Any?) {
        ZapPrintImpl.getInstance().d(tag, any)
    }

    fun i(any: Any?) {
        ZapPrintImpl.getInstance().i(any)
    }

    fun i(tag: String, any: Any?) {
        ZapPrintImpl.getInstance().i(tag, any)
    }

    fun w(any: Any?) {
        ZapPrintImpl.getInstance().w(any)
    }

    fun w(tag: String, any: Any?) {
        ZapPrintImpl.getInstance().w(tag, any)
    }

    fun e(any: Any?) {
        ZapPrintImpl.getInstance().e(any)
    }

    fun e(tag: String, any: Any?) {
        ZapPrintImpl.getInstance().e(tag, any)
    }
}