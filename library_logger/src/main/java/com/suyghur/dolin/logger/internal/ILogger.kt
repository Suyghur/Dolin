package com.suyghur.dolin.logger.internal

/**
 * @author #Suyghur.
 * Created on 4/7/21
 */
interface ILogger {

    fun d(tag: String, msg: String, vararg args: Any)

    fun d(tag: String, any: Any)

    fun i(tag: String, msg: String, vararg args: Any)

    fun w(tag: String, msg: String, vararg args: Any)

    fun e(tag: String, msg: String, vararg args: Any)

    fun e(throwable: Throwable, tag: String, msg: String, vararg args: Any)

    fun record(level: Int, tag: String, msg: String)

    fun print(tag: String, msg: String, vararg args: Any)

    fun print(tag: String, any: Any)

    fun print(level: Int, tag: String, msg: String)

}