package com.suyghur.dolin.zap.crashlytics

import android.os.Process
import com.suyghur.dolin.zap.Zap
import kotlin.system.exitProcess

/**
 * @author #Suyghur.
 * Created on 4/12/21
 */
class JavaCrashCatcher : Thread.UncaughtExceptionHandler {

    private val defaultHandler = Thread.getDefaultUncaughtExceptionHandler()

    private var intercept = false

    fun initialize() {
        try {
            Thread.setDefaultUncaughtExceptionHandler(this)
        } catch (e: Exception) {
            Zap.e("JavaCrashCatcher->setDefaultUncaughtExceptionHandler() error , ${e.localizedMessage}")
        }
    }

    override fun uncaughtException(t: Thread, e: Throwable) {
        if (defaultHandler != null) {
            Thread.setDefaultUncaughtExceptionHandler(defaultHandler)
        }
        try {
            handleException(t, e)
        } catch (e: Exception) {
            Zap.e("JavaCrashCatcher->handlerException error , ${e.localizedMessage}")
        }
        if (intercept) {
            Process.killProcess(Process.myPid())
            exitProcess(10)
        } else {
            defaultHandler?.apply {
                uncaughtException(t, e)
            }
        }

    }

    private fun handleException(t: Thread, e: Throwable) {

    }


    companion object {

        fun getInstance(): JavaCrashCatcher {
            return JavaCrashCatcherHolder.INSTANCE
        }

        private object JavaCrashCatcherHolder {
            val INSTANCE = JavaCrashCatcher()
        }

        /**
         * 防止单例对象在反序列化时重新生成对象
         */
        private fun readResolve(): Any {
            return JavaCrashCatcherHolder.INSTANCE
        }
    }

}