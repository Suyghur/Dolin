package com.dolin.zap.util

import java.io.File

/**
 * @author #Suyghur.
 * Created on 4/15/2021
 */
object LogFileUtils {

    fun cleanOverdueLog(folderPath: String, overdueDayMs: Long) {
        val folder = File(folderPath)
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

    fun getLogFileNumByDate(folderPath: String, date: String): Int {
        var num = 0
        val folder = File(folderPath)
        if (!folder.exists()) {
            return num
        }
        repeat(folder.walk().maxDepth(1)
                .filter { it.isFile }
                .filter { it.name.startsWith(date) }
                .count()) {
            num++
        }
        return num
    }
}