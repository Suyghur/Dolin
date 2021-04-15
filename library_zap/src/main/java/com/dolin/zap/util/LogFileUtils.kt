package com.dolin.zap.util

import android.content.Context
import java.io.File

/**
 * @author #Suyghur.
 * Created on 4/15/2021
 */
object LogFileUtils {

    fun cleanOverdueLog(context: Context, logFolderPath: String, overdueDayMs: Long) {
        val folder = File(logFolderPath)
        if (!folder.exists()) {
            return
        }
        folder.walk().maxDepth(1)
                .filter { it.isFile }
                .filter { it.extension == "zap" }
                .forEach {
                    if (isFileOverdue(it, overdueDayMs)) {
                        it.delete()
                    }
                }
    }

    private fun isFileOverdue(file: File, overdueDayMs: Long): Boolean {
        return System.currentTimeMillis() - file.lastModified() > overdueDayMs
    }
}