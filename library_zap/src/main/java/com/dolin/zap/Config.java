package com.dolin.zap;

/**
 * @author #Suyghur.
 * Created on 2021/05/19
 */
public class Config {

    public String logFolderDir;
    public String tag;
    public Level logcatLevel;
    public Level recordLevel;
    public boolean recordEnable;
    public boolean compressEnable;
    public long overdueDayMs;
    public int fileSizeLimitDayByte;

    private Config(Builder builder) {
        this.logFolderDir = builder.logFolderDir;
        this.tag = builder.tag;
        this.logcatLevel = builder.logcatLevel;
        this.recordLevel = builder.recordLevel;
        this.recordEnable = builder.recordEnable;
        this.compressEnable = builder.compressEnable;
        this.overdueDayMs = builder.overdueDay * 24 * 3600 * 1000L;
        this.fileSizeLimitDayByte = builder.fileSizeLimitDay * 1024 * 1024;
    }

    @Override
    public String toString() {
        return "Config{" +
                "logFolderDir='" + logFolderDir + '\'' +
                ", tag='" + tag + '\'' +
                ", logcatLevel=" + logcatLevel +
                ", recordLevel=" + recordLevel +
                ", recordEnable=" + recordEnable +
                ", compressEnable=" + compressEnable +
                ", overdueDayMs=" + overdueDayMs +
                ", fileSizeLimitDayByte=" + fileSizeLimitDayByte +
                '}';
    }

    public static final class Builder {
        private String logFolderDir = "";
        private String tag = "";
        private Level logcatLevel = Level.DEBUG;
        private Level recordLevel = Level.DEBUG;
        private boolean recordEnable = true;
        private boolean compressEnable = true;
        private long overdueDay = 3;
        private int fileSizeLimitDay = 15;

        /**
         * 日志存储路径，默认是应用的私有目录下dolin文件夹
         */
        public Builder setLogFolderDir(String logFolderDir) {
            this.logFolderDir = logFolderDir;
            return this;
        }

        public Builder setTag(String tag) {
            this.tag = tag;
            return this;
        }

        /**
         * 允许输出到logcat的日志的最低级别，默认为DEBUG级别
         */
        public Builder setLogcatLevel(Level logcatLevel) {
            this.logcatLevel = logcatLevel;
            return this;
        }

        /**
         * 允许记录到文件的日志的最低级别, 默认为DEBUG级别
         */
        public Builder setRecordLevel(Level recordLevel) {
            this.recordLevel = recordLevel;
            return this;
        }

        public Builder setRecordEnable(boolean recordEnable) {
            this.recordEnable = recordEnable;
            return this;
        }

        public Builder setCompressEnable(boolean compressEnable) {
            this.compressEnable = compressEnable;
            return this;
        }

        /**
         * 日志过期天数，过期日志将自动清除，默认为3
         */
        public Builder setOverdueDay(long overdueDay) {
            this.overdueDay = overdueDay;
            return this;
        }

        /**
         * 单天日志文件存储上限，单位 MB，默认为15
         */
        public Builder setFileSizeLimitDay(int fileSizeLimitDay) {
            this.fileSizeLimitDay = fileSizeLimitDay;
            return this;
        }

        public Config create() {
            return new Config(this);
        }
    }
}
