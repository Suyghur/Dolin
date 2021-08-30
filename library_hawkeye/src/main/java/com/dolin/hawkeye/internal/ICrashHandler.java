package com.dolin.hawkeye.internal;

/**
 * @author #Suyghur.
 * Created on 2021/07/30
 */
public interface ICrashHandler {

    void onCrash(String logPath, String stackInfo);
}
