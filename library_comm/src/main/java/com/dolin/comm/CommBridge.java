package com.dolin.comm;

/**
 * @author #Suyghur.
 * Created on 2021/07/28
 */
public class CommBridge {

    static {
        System.loadLibrary("dolin-comm");
    }

    public native static String nativeInvoke(String method);
}
