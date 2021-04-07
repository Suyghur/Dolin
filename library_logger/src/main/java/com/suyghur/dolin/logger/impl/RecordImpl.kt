package com.suyghur.dolin.logger.impl

import com.suyghur.dolin.logger.internal.IRecord

/**
 * @author #Suyghur.
 * Created on 4/7/21
 */
class RecordImpl(bufferPath: String, capacity: Int, logPath: String, compress: Boolean) : IRecord {

    //句柄
    private var ptr = 0L

    init {
        System.loadLibrary("logkit")
        try {
            ptr = initNative(bufferPath, capacity, logPath, compress)
        } catch (e: Exception) {
            e.printStackTrace()
        }
    }

    override fun write(msg: String) {
        if (ptr != 0L) {
            try {
                writeNative(ptr, msg)
            } catch (e: Exception) {
                e.printStackTrace()
            }
        }
    }

    override fun asyncFlush() {
        if (ptr != 0L) {
            try {
                asyncFlushNative(ptr)
            } catch (e: Exception) {
                e.printStackTrace()
            }
        }
    }

    override fun changeLogPath(path: String) {
        if (ptr != 0L) {
            try {
                changeLogPathNative(ptr, path)
            } catch (e: Exception) {
                e.printStackTrace()
            }
        }
    }

    override fun release() {
        if (ptr != 0L) {
            try {
                releaseNative(ptr)
            } catch (e: Exception) {
                e.printStackTrace()
            }
        }
    }

    private external fun writeNative(ptr: Long, msg: String)

    private external fun asyncFlushNative(ptr: Long)

    private external fun changeLogPathNative(ptr: Long, path: String)

    private external fun releaseNative(ptr: Long)

    companion object {
        @JvmStatic
        private external fun initNative(bufferPath: String, capacity: Int, logPath: String, compress: Boolean): Long
    }
}