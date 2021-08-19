package com.dolin.hawkeye.handler;

/**
 * @author #Suyghur.
 * Created on 2021/08/06
 */
public class AnrCrashHandler {

    private AnrCrashHandler() {

    }

    public static AnrCrashHandler getInstance() {
        return AnrCrashHandlerHolder.INSTANCE;
    }

    private static class AnrCrashHandlerHolder {
        private static final AnrCrashHandler INSTANCE = new AnrCrashHandler();
    }
}
