package com.dolin.zap.impl

import android.util.Log
import com.dolin.zap.internal.IRecord

/**
 * @author #Suyghur.
 * Created on 4/7/21
 */
class Record2MMap(bufferPath: String, capacity: Int, private val logPath: String, compress: Boolean, limitSize: Int) : IRecord {

    //buffer指针
    private var ptr = 0L

    init {
        System.loadLibrary("dolin-zap")
        try {
            ptr = initNative(bufferPath, capacity, logPath, compress, limitSize)
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
                Log.d("dolin_zap", "log path $logPath")
                if (isLogFileOverSizeNative(ptr)) {
                    //新建文件扩展文件
//                    changeLogPathNative()
                }
                asyncFlushNative(ptr)
            } catch (e: Exception) {
                e.printStackTrace()
            }
        }
    }

    override fun expLogFile(path: String, partNum: Int) {
        if (ptr != 0L) {
            try {
                expLogFileNative(ptr, path, partNum)
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

    private external fun expLogFileNative(ptr: Long, path: String, partNum: Int)

    private external fun releaseNative(ptr: Long)

    private external fun getPartNumNative(ptr: Long): Int

    private external fun isLogFileOverSizeNative(ptr: Long): Boolean

    companion object {
        @JvmStatic
        private external fun initNative(bufferPath: String, capacity: Int, logPath: String, compress: Boolean, limitSize: Int): Long
    }
}