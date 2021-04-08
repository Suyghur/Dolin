package com.suyghur.dolin.zap.internal

/**
 * @author #Suyghur.
 * Created on 4/7/21
 */
interface ILogger {


    fun d(any: Any?)

    fun d(tag: String, any: Any?)

    fun i(any: Any?)

    fun i(tag: String, any: Any?)

    fun w(any: Any?)

    fun w(tag: String, any: Any?)

    fun e(any: Any?)

    fun e(tag: String, any: Any?)

    fun record(level: Int, tag: String, msg: String)

}