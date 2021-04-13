package com.suyghur.dolin.common

/**
 * @author #Suyghur.
 * Created on 4/12/21
 */
object Common {

    init {
        System.loadLibrary("dolin-common")
    }

    external fun testLib(): String
}