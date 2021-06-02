package com.dolin.zap;

import android.app.Application;

import com.dolin.zap.entity.Config;
import com.dolin.zap.impl.ZapPrint;

/**
 * @author #Suyghur.
 * Created on 2021/05/19
 */
public class Zap {

    private static boolean hasInitialized = false;

    public static void initialize(Application application, Config config) {
        ZapPrint.getInstance().initialize(application, config);
        hasInitialized = true;
    }

    public static void recycle() {
        if (hasInitialized) {
            ZapPrint.getInstance().recycle();
        }
    }

    public static void d(Object obj) {
        if (hasInitialized) {
            ZapPrint.getInstance().d(obj);
        }
    }

    public static void d(String tag, Object obj) {
        if (hasInitialized) {
            ZapPrint.getInstance().d(tag, obj);
        }
    }

    public static void i(Object obj) {
        if (hasInitialized) {
            ZapPrint.getInstance().i(obj);
        }
    }

    public static void i(String tag, Object obj) {
        if (hasInitialized) {
            ZapPrint.getInstance().i(tag, obj);
        }
    }

    public static void w(Object obj) {
        if (hasInitialized) {
            ZapPrint.getInstance().w(obj);
        }
    }

    public static void w(String tag, Object obj) {
        if (hasInitialized) {
            ZapPrint.getInstance().w(tag, obj);
        }
    }

    public static void e(Object obj) {
        if (hasInitialized) {
            ZapPrint.getInstance().e(obj);
        }
    }

    public static void e(String tag, Object obj) {
        if (hasInitialized) {
            ZapPrint.getInstance().e(tag, obj);
        }
    }

    public static String getVersion() {
        return Version.VERSION;
    }

    public static String getFullVersion() {
        return Version.FULL_VERSION;
    }
}
