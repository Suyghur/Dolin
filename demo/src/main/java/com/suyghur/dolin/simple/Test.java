package com.suyghur.dolin.simple;

import android.app.Activity;
import android.util.Log;

import com.suyghur.dolin.logger.Config;

/**
 * @author #Suyghur.
 * Created on 4/7/21
 */
public class Test {


    String logDir = "";
    private int logcatLevel = -1;
    private int recordLevel = -1;
    private long overdueDayMs = 0L;
    private int fileSizeLimitDayByte = 0;


    @Override
    public String toString() {
//        Log.DEBUG
        return "Test{" +
                "logDir='" + logDir + '\'' +
                ", logcatLevel=" + logcatLevel +
                ", recordLevel=" + recordLevel +
                ", overdueDayMs=" + overdueDayMs +
                ", fileSizeLimitDayByte=" + fileSizeLimitDayByte +
                '}';
    }
}
