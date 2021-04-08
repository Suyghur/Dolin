package com.suyghur.dolin.zap.entity

/**
 * 参考[android.os.Message]的享元模式
 * @author #Suyghur.
 * Created on 4/8/21
 */
class ZapData {

    var level: Level = Level.NONE
    var tag = ""
    var msg: String = ""
    private var next: ZapData? = null

    fun recycle() {
        level = Level.NONE
        tag = ""
        msg = ""
        synchronized(sPoolSync) {
            if (sPoolSize < MAX_POOL_SIZE) {
                next = sPool
                sPool = this
                sPoolSize++
            }
        }
    }

    companion object {
        private val sPoolSync = Any()
        private var sPool: ZapData? = null
        private var sPoolSize = 0
        private const val MAX_POOL_SIZE = 50

        @JvmStatic
        fun obtain(): ZapData {
            synchronized(sPoolSync) {
                if (sPool != null) {
                    val data = sPool
                    sPool = data!!.next
                    data.next = null
                    sPoolSize--
                    return data
                }
            }
            return ZapData()
        }

        @JvmStatic
        fun obtain(level: Level, tag: String, msg: String): ZapData {
            val data = obtain()
            data.level = level
            data.tag = tag
            data.msg = msg
            return data
        }
    }
}