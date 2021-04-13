package com.dolin.common.util

import android.app.ActivityManager
import android.content.Context
import android.os.Build
import android.provider.Settings
import android.text.TextUtils
import java.io.BufferedReader
import java.io.FileReader
import kotlin.math.ceil

/**
 * @author #Suyghur.
 * Created on 4/12/21
 */
object DeviceInfoUtils {


    /**
     * 获取设备ID（AndroidId）
     */
    fun getAndroidDeviceId(context: Context): String {
        val id = Settings.Secure.getString(context.contentResolver, Settings.Secure.ANDROID_ID)
        return if (TextUtils.isEmpty(id)) {
            ""
        } else {
            id
        }
    }

    /**
     * 获取手机厂商
     */
    fun getDeviceBrand(): String {
        return Build.BRAND
    }

    /**
     * 获取手机型号
     */
    fun getModel(): String {
        return Build.MODEL
    }

    /**
     * 获取系统版本
     */
    fun getDeviceSoftWareVersion(): String {
        return Build.VERSION.RELEASE
    }

    /**
     * 获取cpu核数
     */
    fun getCpuCount(): String {
        return "${Runtime.getRuntime().availableProcessors()}"
    }

    /**
     * 获取cpu架构
     */
    fun getCpuABI(): String {
        val abis: Array<String> = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            Build.SUPPORTED_ABIS
        } else {
            arrayOf(Build.CPU_ABI, Build.CPU_ABI2)
        }
        val sb = StringBuilder()
        for (index in abis.indices) {
            sb.append(abis[index])
            if (index != abis.size - 1) {
                sb.append(" , ")
            }
        }
        return sb.toString()
    }

    /**
     * 获取手机运行内存RAM
     */
    fun getRAM(): String {
        var firstLine = ""
        BufferedReader(FileReader("/proc/meminfo"), 8192).use {
            firstLine = it.readLine().split("\\s+".toRegex())[1]
        }
        return if (TextUtils.isEmpty(firstLine)) {
            "0GB"
        } else {
            "${ceil(firstLine.toFloat() / (1024 * 1024).toDouble()).toInt()}GB"
        }
    }

    /**
     * 系统空闲内存（单位M）
     */
    fun getAvailMem(context: Context): Int {
        val am = context.getSystemService(Context.ACTIVITY_SERVICE) as ActivityManager
        val mi = ActivityManager.MemoryInfo()
        am.getMemoryInfo(mi)
        //mi.availMem
        return (mi.availMem / 1024 / 1024).toInt()
    }

}