package com.suyghur.dolin

import android.annotation.SuppressLint
import android.content.Context
import android.os.Build
import android.os.Debug
import android.text.TextUtils
import androidx.annotation.Keep
import androidx.annotation.RequiresApi
import com.suyghur.dolin.toolkit.LogKit
import java.io.File
import java.nio.file.Files
import java.nio.file.Paths

/**
 * @author #Suyghur.
 * Created on 4/2/21
 */
class Zege {

    private var ptr = 0L


    @SuppressLint("UnsafeDynamicallyLoadedCode")
    fun initialize(context: Context) {
        if (hasInitialized) {
            LogKit.e("Zege sdk has been initialized")
            return
        }
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.O) {
            LogKit.e("Zege sdk failed to initialize, at least support Android 8.0 version")
            return
        }
        val path = staticSoFile(context)
        hasInitialized = if (!TextUtils.isEmpty(path)) {
            System.loadLibrary("zege")
            attachJvmtiAgent(context, path)
            true
        } else {
            false
        }
    }

    @RequiresApi(Build.VERSION_CODES.O)
    private fun staticSoFile(context: Context): String {
        try {
            val packageCodePath = context.packageCodePath
            val clzLoader = context.classLoader
            val findLibraryMethod = ClassLoader::class.java.getDeclaredMethod("findLibrary", String::class.java)
            val jvmtiAgentLibPath: String = findLibraryMethod.invoke(clzLoader, "zege") as String

            LogKit.d("lib path $jvmtiAgentLibPath")
            val filesDir = context.filesDir
            val jvmtiLibDir = File(filesDir, "zege")
            if (!jvmtiLibDir.exists()) {
                jvmtiLibDir.mkdirs()
            }
            val agentLibSo = File(jvmtiLibDir, "agent.so")
            if (agentLibSo.exists()) {
                agentLibSo.delete()
            }
            Files.copy(Paths.get(File(jvmtiAgentLibPath).absolutePath), Paths.get(agentLibSo.absolutePath))
            LogKit.d("${agentLibSo.absolutePath}, $packageCodePath")
            return agentLibSo.absolutePath
        } catch (e: Exception) {
            LogKit.e(e.localizedMessage)
            e.printStackTrace()
        }
        return ""
    }


    private fun attachJvmtiAgent(context: Context, agentPath: String) {
        try {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
                Debug.attachJvmtiAgent(agentPath, null, context.classLoader)
            } else {
                val vmDebugClz = Class.forName("dalvik.system.VMDebug")
                val attachAgentMethod = vmDebugClz.getMethod("attachAgent", String::class.java)
                attachAgentMethod.isAccessible = true
                attachAgentMethod.invoke(null, agentPath)
            }
        } catch (e: Exception) {
            LogKit.e(e.localizedMessage)
            e.printStackTrace()
        }
    }

    private external fun initNative(): Long

//    private external fun getObjectSize(ptr: Long, any: Any)


    companion object {

        @Volatile
        private var hasInitialized = false

        @JvmStatic
        @Keep
        fun getDefault(): Zege {
            return ZegeHolder.INSTANCE
        }

        private object ZegeHolder {
            val INSTANCE = Zege()
        }

    }
}