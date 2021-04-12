package com.suyghur.dolin.common.util

import android.content.Context
import java.io.File

/**
 * @author #Suyghur.
 * Created on 4/9/21
 */
object FileUtils {

    fun getLogDir(context: Context): String {
        var path = context.getExternalFilesDir("dolin/zap")
        if (path == null) {
            path = File(context.filesDir, "dolin/zap")
        }
        if (!path.exists()) {
            path.mkdirs()
        }
        return path.absolutePath

    }
}