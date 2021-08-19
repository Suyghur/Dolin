package com.dolin.comm.util;

import android.content.Context;
import android.provider.Settings;

import com.dolin.comm.impl.NativeBridge;

/**
 * @author #Suyghur.
 * Created on 2021/07/28
 */
public class DeviceInfoUtils {

    public static String getAndroidDeviceId(Context context) {
        return Settings.Secure.getString(context.getContentResolver(), Settings.Secure.ANDROID_ID);
    }

    public static String getDeviceManufacturer() {
        return NativeBridge.nativeInvoke("GetDeviceManufacturer");
    }

    public static String getMobileBrand() {
        return NativeBridge.nativeInvoke("GetMobileBrand");
    }

    public static String getDeviceModel() {
        return NativeBridge.nativeInvoke("GetDeviceModel");
    }

    public static String getDeviceSoftWareVersion() {
        return NativeBridge.nativeInvoke("GetDeviceSoftwareVersion");
    }

    public static String getCpuCount() {
        return Runtime.getRuntime().availableProcessors() + "";
    }

    public static String getCpuAbi() {
        return NativeBridge.nativeInvoke("GetCpuAbi");
    }

    public static String getRAM() {
        return "";
    }

    public static int getAvailMem(Context context) {
        return 0;
    }
}
