package com.dolin.hawkeye.utils;

import android.os.Build;
import android.text.TextUtils;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

/**
 * @author #Suyghur.
 * Created on 2021/08/24
 */
public class DeviceUtils {

    public static String getAbiList() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            return TextUtils.join(",", Build.SUPPORTED_ABIS);
        } else {
            String abi = Build.CPU_ABI;
            String abi2 = Build.CPU_ABI2;
            if (TextUtils.isEmpty(abi2)) {
                return abi;
            } else {
                return abi + "," + abi2;
            }
        }
    }

    public static String getDeviceModel() {
        String mobileModel = null;
        if (isMIUI()) {
            String deviceName = "";

            try {
                Class<?> systemProperties = Class.forName("android.os.SystemProperties");
                Method get = systemProperties.getDeclaredMethod("get", String.class, String.class);
                deviceName = (String) get.invoke(systemProperties, "ro.product.marketname", "");
                if (TextUtils.isEmpty(deviceName)) {
                    deviceName = (String) get.invoke(systemProperties, "ro.product.model", "");
                }
            } catch (InvocationTargetException e) {
                e.printStackTrace();
            } catch (NoSuchMethodException e) {
                e.printStackTrace();
            } catch (IllegalAccessException e) {
                e.printStackTrace();
            } catch (ClassNotFoundException e) {
                e.printStackTrace();
            }
            mobileModel = deviceName;
        } else {
            mobileModel = Build.MODEL;
        }
        if (mobileModel == null) {
            mobileModel = "";
        }
        return mobileModel;
    }

    public static boolean isMIUI() {
        String property = getSystemProperty("ro.miui.ui.version.name", "");
        return !TextUtils.isEmpty(property);
    }

    public static String getSystemProperty(String key, String defaultValue) {
        try {
            Class<?> clz = Class.forName("android.os.SystemProperties");
            Method get = clz.getMethod("get", String.class, String.class);
            return (String) get.invoke(clz, key, defaultValue);
        } catch (NoSuchMethodException var4) {
            var4.printStackTrace();
        } catch (IllegalAccessException var5) {
            var5.printStackTrace();
        } catch (InvocationTargetException var6) {
            var6.printStackTrace();
        } catch (ClassNotFoundException var7) {
            var7.printStackTrace();
        }

        return defaultValue;
    }
}
