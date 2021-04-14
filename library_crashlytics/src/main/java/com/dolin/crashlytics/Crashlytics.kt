package com.dolin.crashlytics

import android.app.Application
import com.dolin.crashlytics.entity.Config

/**
 * @author #Suyghur.
 * Created on 4/14/21
 */
class Crashlytics {

    fun initialize(application: Application, config: Config) {

    }

    companion object {

        @JvmStatic
        fun getVersion(): String {
            return Version.VERSION
        }

        @JvmStatic
        fun getFullVersion(): String {
            return Version.FULL_VERSION
        }

        @JvmStatic
        fun getInstance(): Crashlytics {
            return CrashlyticsHolder.INSTANCE
        }

        private object CrashlyticsHolder {
            val INSTANCE = Crashlytics()
        }
    }
}