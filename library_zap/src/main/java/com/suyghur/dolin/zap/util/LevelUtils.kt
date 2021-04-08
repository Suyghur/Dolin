package com.suyghur.dolin.zap.util

import com.suyghur.dolin.zap.entity.Level

/**
 * @author #Suyghur.
 * Created on 4/8/21
 */
object LevelUtils {

    fun getLevelName(level: Level): String {
        return level.name
    }

    fun getShortLevelName(level: Level): String {
        return when (level) {
            Level.DEBUG -> "D"
            Level.INFO -> "I"
            Level.WARNING -> "W"
            Level.ERROR -> "E"
            else ->
                if (level.ordinal < Level.DEBUG.ordinal) {
                    "D"
                } else {
                    "E"
                }
        }
    }
}