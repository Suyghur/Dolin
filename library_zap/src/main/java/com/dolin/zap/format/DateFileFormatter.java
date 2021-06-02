package com.dolin.zap.format;

import android.text.TextUtils;

import com.dolin.zap.entity.Level;
import com.dolin.zap.internal.IFormatter;
import com.dolin.zap.util.LevelUtils;

import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;
import java.util.Locale;

/**
 * @author #Suyghur.
 * Created on 2021/05/19
 */
public class DateFileFormatter implements IFormatter {

    private SimpleDateFormat simpleDateFormat;
    private Date date = null;
    private String lastDateFormatted = "";
    private StringBuffer stringBuffer;
    private int timeLength = 0;

    public DateFileFormatter() {
        this("yyyy-MM-dd HH:mm:ss");
    }

    public DateFileFormatter(String pattern) {
        simpleDateFormat = new SimpleDateFormat(pattern, Locale.getDefault());
        stringBuffer = new StringBuffer();
        //重置秒数
        Calendar.getInstance().set(Calendar.SECOND, 0);
        date = Calendar.getInstance().getTime();
    }

    private void resetTimePrefix() {
        if (stringBuffer == null) {
            return;
        }
        if (stringBuffer.length() > 0) {
            stringBuffer.delete(0, stringBuffer.length());
        }
        timeLength = stringBuffer.append(lastDateFormatted).append(" ").length();
    }

    private String formatString(Level level, String tag, String msg) {
        if (stringBuffer == null) {
            return "";
        }
        if (stringBuffer.length() > timeLength) {
            stringBuffer.delete(timeLength, stringBuffer.length());
        }
        return stringBuffer.append("[").append(LevelUtils.getShortLevelName(level)).append("]")
                .append(tag).append(": ").append(msg).append("\n").toString();
    }

    @Override
    public synchronized String format(Level level, String tag, String msg) {
        if ((System.currentTimeMillis() - date.getTime()) > 1000 || TextUtils.isEmpty(lastDateFormatted)) {
            date.setTime(System.currentTimeMillis());
            lastDateFormatted = simpleDateFormat.format(date);
            resetTimePrefix();
        }
        return formatString(level, tag, msg);
    }
}
