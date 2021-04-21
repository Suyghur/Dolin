package com.dolin.zap.impl

import com.dolin.zap.internal.IRecord
import com.dolin.zap.util.LogFileUtils
import java.io.File

/**
 * @author #Suyghur.
 * Created on 4/7/21
 */
class Record2MMap(bufferDir: String,private val logDir:String, private val logDate: String,
                  capacity: Int, private val limitSize: Int, compress: Boolean) : IRecord {

    //buffer指针
    private var ptr = 0L
    private var logPath: String
    private var num = 1

    init {
        System.loadLibrary("dolin-zap")
        num = LogFileUtils.getLogFileNumByDate(logDir, logDate)
        logPath = if (num > 1) {
            "$logDir/$logDate-p$num.zap"
        } else {
            "$logDir/$logDate.zap"
        }
        try {
            ptr = initNative(bufferDir, logPath, capacity, limitSize, compress)
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
                //自动扩容
                if (isLogFileOverSizeNative(ptr)) {
                    logPath = "$logDir/$logDate-p${num + 1}.zap"
                    expLogFileNative(ptr, logPath, limitSize)
                }
                asyncFlushNative(ptr)
            } catch (e: Exception) {
                e.printStackTrace()
            }
        }
    }

    override fun expLogFile(path: String, limitSize: Int) {
        if (ptr != 0L) {
            try {
                expLogFileNative(ptr, path, limitSize)
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

    private external fun expLogFileNative(ptr: Long, path: String, limitSize: Int)

    private external fun releaseNative(ptr: Long)

    private external fun isLogFileOverSizeNative(ptr: Long): Boolean

    companion object {
        @JvmStatic
        private external fun initNative(bufferPath: String, logPath: String, capacity: Int, limitSize: Int, compress: Boolean): Long
    }
}