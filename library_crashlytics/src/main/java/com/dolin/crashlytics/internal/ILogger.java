package com.dolin.crashlytics.internal;

/**
 * @author #Suyghur.
 * Created on 2021/07/30
 */
public interface ILogger {

    /**
     * Log a DEBUG message.
     *
     * @param tag Used to identify the source of a log message.
     * @param msg The message you would like logged.
     */
    @SuppressWarnings("unused")
    void d(String tag, String msg);

    /**
     * Log a INFO message.
     *
     * @param tag Used to identify the source of a log message.
     * @param msg The message you would like logged.
     */
    @SuppressWarnings("unused")
    void i(String tag, String msg);


    /**
     * Log a WARN message.
     *
     * @param tag Used to identify the source of a log message.
     * @param msg The message you would like logged.
     */
    @SuppressWarnings("unused")
    void w(String tag, String msg);


    /**
     * Log a ERROR message.
     *
     * @param tag Used to identify the source of a log message.
     * @param msg The message you would like logged.
     */
    @SuppressWarnings("unused")
    void e(String tag, String msg);

}
