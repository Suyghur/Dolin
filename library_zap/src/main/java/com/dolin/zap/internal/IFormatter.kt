package com.dolin.zap.internal

import com.dolin.zap.entity.Level

/**
 * @author #Suyghur.
 * Created on 4/8/21
 */
interface IFormatter {
    fun format(level: Level, tag: String, msg: String): String
}