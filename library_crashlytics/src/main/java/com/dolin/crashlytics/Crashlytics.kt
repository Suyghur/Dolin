package com.dolin.crashlytics

/**
 * @author #Suyghur.
 * Created on 4/14/21
 */
class Crashlytics {

    companion object {

        fun getInstance(): Crashlytics {
            return CrashlyticsHolder.INSTANCE
        }

        private object CrashlyticsHolder {
            val INSTANCE = Crashlytics()
        }
    }
}