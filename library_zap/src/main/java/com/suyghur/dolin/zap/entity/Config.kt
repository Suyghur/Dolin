package com.suyghur.dolin.zap.entity

import android.text.TextUtils

/**
 * @author #Suyghur.
 * Created on 4/7/21
 */
class Config private constructor(builder: Builder) {

    var logDir = ""
        private set
    var defaultTag = ""
        private set
    var logcatLevel: Level
        private set
    var recordLevel: Level
        private set
    var recordEnable: Boolean
        private set
    private var overdueDayMs = 0L
    private var fileSizeLimitDayByte = 0


    init {
        this.logDir = builder.logDir
        this.defaultTag = builder.defaultTag
        this.logcatLevel = builder.logcatLevel
        this.recordLevel = builder.recordLevel
        this.recordEnable = builder.recordEnable
        this.overdueDayMs = builder.overdueDay * 24 * 3600 * 1000L
        this.fileSizeLimitDayByte = builder.fileSizeLimitDay * 1024 * 1024
    }

    override fun toString(): String {
        return "Config{" +
                "logDir='" + logDir + '\'' +
                ", logcatLevel=" + logcatLevel +
                ", recordLevel=" + recordLevel +
                ", overdueDayMs=" + overdueDayMs +
                ", fileSizeLimitDayByte=" + fileSizeLimitDayByte +
                '}'
    }

    class Builder() {
        internal var logDir = ""
        internal var defaultTag = "dolin_zap"
        internal var logcatLevel = Level.DEBUG
        internal var recordLevel = Level.DEBUG
        internal var recordEnable = true
        internal var overdueDay = 3
        internal var fileSizeLimitDay = 15

        /**
         * 日志存储路径，默认是应用的私有目录下dolin文件夹
         */
        fun setLogDir(logDir: String): Builder {
            this.logDir = logDir
            return this
        }

        fun setDefaultTag(tag: String): Builder {
            if (!TextUtils.isEmpty(tag)) {
                this.defaultTag = tag
            }
            return this
        }

        /**
         * 允许输出到logcat的日志的最低级别，默认为[Level.DEBUG]级别
         */
        fun setLogcatLevel(level: Level): Builder {
            this.logcatLevel = level
            return this
        }

        /**
         * 允许记录到文件的日志的最低级别, 默认为[Level.DEBUG]级别
         */
        fun setRecordLevel(level: Level): Builder {
            this.recordLevel = level
            return this
        }

        fun setRecordEnable(enable: Boolean): Builder {
            this.recordEnable = enable
            return this
        }

        /**
         * 日志过期天数，过期日志将自动清除，默认为3
         */
        fun setOverdueDay(overdueDay: Int): Builder {
            this.overdueDay = overdueDay
            return this
        }

        /**
         * 单天日志文件存储上限，单位 MB，默认为15
         */
        fun setFileSizeLimitDay(mb: Int): Builder {
            this.fileSizeLimitDay = mb
            return this
        }

        fun create(): Config {
            return Config(this)
        }
    }
}