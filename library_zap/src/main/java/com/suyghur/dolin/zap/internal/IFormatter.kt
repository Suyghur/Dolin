package com.suyghur.dolin.zap.internal

import com.suyghur.dolin.zap.entity.Level

/**
 * @author #Suyghur.
 * Created on 4/8/21
 */
interface IFormatter {
    fun format(level: Level, tag: String, msg: String): String
}