package com.suyghur.dolin.zap.internal

/**
 * @author #Suyghur.
 * Created on 4/7/21
 */
interface IRecord {

    fun write(msg: String)

    fun asyncFlush()

    fun changeLogPath(path:String)

    fun release()
}