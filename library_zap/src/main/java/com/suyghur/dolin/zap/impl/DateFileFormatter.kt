package com.suyghur.dolin.zap.impl

import android.text.TextUtils
import com.suyghur.dolin.zap.entity.Level
import com.suyghur.dolin.zap.internal.IFormatter
import com.suyghur.dolin.zap.util.LevelUtils
import java.text.SimpleDateFormat
import java.util.*

/**
 * @author #Suyghur.
 * Created on 4/8/21
 */
class DateFileFormatter : IFormatter {

    private val pattern = "yyy-MM-dd HH:mm:ss"
    private var simpleDateFormat: SimpleDateFormat = SimpleDateFormat(pattern, Locale.getDefault())
    private var date: Date
    private var lastDateFormatted = ""
    private var stringBuilder: StringBuilder = StringBuilder()
    private var timeLength = 0

    init {
        Calendar.getInstance()[Calendar.SECOND] = 0
        date = Calendar.getInstance().time
    }

    private fun resetTimePrefix() {
        if (stringBuilder.isNotEmpty()) {
            stringBuilder.delete(0, stringBuilder.length)
        }
        timeLength = stringBuilder.append(lastDateFormatted).append(" ").length
    }

    private fun formatString(level: Level, tag: String, msg: String): String {
        if (stringBuilder.length > timeLength) {
            stringBuilder.delete(timeLength, stringBuilder.length)
        }
        return stringBuilder.append("[${LevelUtils.getShortLevelName(level)}").append(tag).append(": ").append(msg).append("\n").toString()
    }

    override fun format(level: Level, tag: String, msg: String): String {
        if ((System.currentTimeMillis() - date.time) > 1000 || TextUtils.isEmpty(lastDateFormatted)) {
            date.time = System.currentTimeMillis()
            lastDateFormatted = simpleDateFormat.format(date)
            resetTimePrefix()
        }
        return formatString(level, tag, msg)
    }
}