package com.dolin.zap.impl;

import android.app.Application;
import android.text.TextUtils;
import android.util.Log;

import com.dolin.zap.entity.Config;
import com.dolin.zap.entity.Level;
import com.dolin.zap.entity.ZapData;
import com.dolin.zap.format.DateFileFormatter;
import com.dolin.zap.internal.IPrint;
import com.dolin.zap.lifecycle.ZapLifecycle;
import com.dolin.zap.util.LogFileUtils;

import java.io.File;
import java.lang.reflect.Array;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;

/**
 * @author #Suyghur.
 * Created on 2021/05/19
 */
public class ZapPrint implements IPrint {

    private static final int MAX_LENGTH_OF_SINGLE_MESSAGE = 4063;
    private boolean hasInitialized = false;
    private String tag = "";
    private Level logcatLevel = Level.DEBUG;
    private Level recordLevel = Level.DEBUG;
    private Record2MMap record2MMap = null;
    private DateFileFormatter dateFileFormatter = null;

    private ZapPrint() {
        dateFileFormatter = new DateFileFormatter();
    }

    public static ZapPrint getInstance() {
        return ZapPrintHolder.INSTANCE;
    }

    /**
     * 防止单例对象在反序列化时重新生成对象
     */
    private static Object readResolve() {
        return ZapPrintHolder.INSTANCE;
    }

    public void initialize(final Application application, final Config config) {
        if (hasInitialized) {
            return;
        }
        String date = new SimpleDateFormat("yyyy-MM-dd", Locale.getDefault()).format(new Date());

        String logFolderDir = "";
        if (TextUtils.isEmpty(config.logFolderDir)) {
            logFolderDir = LogFileUtils.getLogFolderDir(application);
        } else {
            logFolderDir = config.logFolderDir;
        }

        if (TextUtils.isEmpty(config.tag)) {
            this.tag = "dolin_zap";
        } else {
            this.tag = config.tag;
        }

        this.logcatLevel = config.logcatLevel;
        this.recordLevel = config.recordLevel;

        String bufferPath = logFolderDir + File.separator + "zap.cache";
        String logPath = LogFileUtils.getLogDir(logFolderDir, date);

        if (config.recordEnable) {
            record2MMap = new Record2MMap(bufferPath, logPath, date, 1024 * 400, config.fileSizeLimitDayByte, config.compressEnable);
            ZapLifecycle.getInstance().registerZapLifeCallback(application, record2MMap);
        }

        new Thread(new Runnable() {
            @Override
            public void run() {
                LogFileUtils.cleanOverdueLog(application, config.overdueDayMs);
            }
        }).start();

        hasInitialized = true;
    }


    @Override
    public void d(Object obj) {
        d(tag, obj);
    }

    @Override
    public void d(String tag, Object obj) {
        print(Level.DEBUG, tag, obj);
    }

    @Override
    public void i(Object obj) {
        i(tag, obj);
    }

    @Override
    public void i(String tag, Object obj) {
        print(Level.INFO, tag, obj);
    }

    @Override
    public void w(Object obj) {
        w(tag, obj);
    }

    @Override
    public void w(String tag, Object obj) {
        print(Level.WARNING, tag, obj);
    }

    @Override
    public void e(Object obj) {
        e(tag, obj);
    }

    @Override
    public void e(String tag, Object obj) {
        print(Level.ERROR, tag, obj);
    }

    private boolean interceptLogcat(Level level) {
        return level.ordinal() < logcatLevel.ordinal();
    }

    private boolean interceptRecord(Level level) {
        return level.ordinal() < recordLevel.ordinal();
    }

    private void print(Level level, String tag, Object obj) {
        String msg = "";
        if (obj == null) {
            msg = "null";
        } else {
            Class<?> clz = obj.getClass();
            if (clz.isArray()) {
                StringBuilder sb = new StringBuilder(clz.getSimpleName());
                sb.append("[ ");
                for (int i = 0; i < Array.getLength(obj); i++) {
                    if (i != 0) {
                        sb.append(", ");
                    }
                    Object tmp = Array.get(obj, i);
                    sb.append(tmp);
                }
                sb.append(" ]");
                msg = sb.toString();
            } else {
                msg = obj + "";
            }
        }

        ZapData data = ZapData.obtain(level, tag, msg);
        if (!interceptLogcat(level)) {
            printInner(data);
        }
        if (!interceptRecord(level)) {
            doRecord(data);
        }
        data.recycle();
    }

    private void printInner(ZapData data) {
        if (data.msg.length() <= MAX_LENGTH_OF_SINGLE_MESSAGE) {
            doPrint(data.level, data.tag, data.msg);
            return;
        }
        int msgLength = data.msg.length();
        int start = 0;
        int end = start + MAX_LENGTH_OF_SINGLE_MESSAGE;
        while (start < msgLength) {
            doPrint(data.level, data.tag, data.msg.substring(start, end));
            start = end;
            end = Math.min(start + MAX_LENGTH_OF_SINGLE_MESSAGE, msgLength);
        }
    }

    private void doPrint(Level level, String tag, String msg) {
        switch (level) {
            case DEBUG:
                Log.d(tag, msg);
                break;
            case WARNING:
                Log.w(tag, msg);
                break;
            case ERROR:
                Log.e(tag, msg);
                break;
            case NONE:
            case INFO:
            default:
                Log.i(tag, msg);
                break;
        }
    }

    private void doRecord(ZapData data) {
        if (record2MMap != null) {
            record2MMap.write(dateFileFormatter.format(data.level, data.tag, data.msg));
        }
    }

    public void recycle() {
        if (record2MMap != null) {
            ZapLifecycle.getInstance().unregisterZapLifeCallback();
            record2MMap.asyncFlush();
            record2MMap.release();
        }
    }

    private static final class ZapPrintHolder {
        private static final ZapPrint INSTANCE = new ZapPrint();
    }

}
