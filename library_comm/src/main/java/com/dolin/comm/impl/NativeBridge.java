package com.dolin.comm.impl;

/**
 * @author #Suyghur.
 * Created on 2021/07/28
 */
public class NativeBridge {

    static {
        System.loadLibrary("dolin-comm");
    }

    public native static String nativeInvoke(String method);
}
