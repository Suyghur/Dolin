package com.suyghur.dolin.toolkit

import android.util.Log
import java.lang.reflect.Array

/**
 * @author #Suyghur.
 * Created on 4/6/21
 */
object LogKit {

    private const val TAG = "zege_sdk"

    @JvmStatic
    fun d(any: Any?) {
        d(TAG, any)
    }

    @JvmStatic
    fun d(tag: String, any: Any?) {
        print(Log.DEBUG, tag, any)
    }

    @JvmStatic
    fun i(any: Any?) {
        i(TAG, any)
    }

    @JvmStatic
    fun i(tag: String, any: Any?) {
        print(Log.INFO, tag, any)
    }

    @JvmStatic
    fun e(any: Any?) {
        d(TAG, any)
    }

    @JvmStatic
    fun e(tag: String, any: Any?) {
        print(Log.ERROR, tag, any)
    }

    private fun print(level: Int, tag: String, any: Any?) {
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
        when (level) {
            Log.DEBUG -> Log.d(tag, msg)
            Log.ERROR -> Log.e(tag, msg)
            Log.INFO -> Log.i(tag, msg)
            else -> Log.i(tag, msg)
        }
    }
}