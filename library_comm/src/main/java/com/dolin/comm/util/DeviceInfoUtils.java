package com.dolin.comm.util;

import android.content.Context;
import android.provider.Settings;

import com.dolin.comm.CommBridge;

/**
 * @author #Suyghur.
 * Created on 2021/07/28
 */
public class DeviceInfoUtils {

    public static String getAndroidDeviceId(Context context) {
        return Settings.Secure.getString(context.getContentResolver(), Settings.Secure.ANDROID_ID);
    }


    public static String getDeviceManufacturer(){
        return CommBridge.nativeInvoke("GetDeviceManufacturer");
    }

    public static String getMobileBrand() {
        return CommBridge.nativeInvoke("GetMobileBrand");
    }

    public static String getDeviceModel() {
        return CommBridge.nativeInvoke("GetDeviceModel");
    }

    public static String getDeviceSoftWareVersion() {
        return CommBridge.nativeInvoke("GetDeviceSoftwareVersion");
    }

    public static String getCpuCount() {
        return Runtime.getRuntime().availableProcessors() + "";
    }

    public static String getCpuAbi() {
        return CommBridge.nativeInvoke("GetCpuAbi");
    }

    public static String getRAM() {
        return "";
    }

    public static int getAvailMem(Context context) {
        return 0;
    }
}
