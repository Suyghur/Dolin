package com.dolin.crashlytics.catcher

/**
 * @author #Suyghur.
 * Created on 4/14/21
 */
class JavaCrashCatcher {





    companion object {

        fun getInstance(): JavaCrashCatcher {
            return JavaCrashCatcherHolder.INSTANCE
        }

        private object JavaCrashCatcherHolder {
            val INSTANCE = JavaCrashCatcher()
        }

        /**
         * 防止单例对象在反序列化时重新生成对象
         */
        private fun readResolve(): Any {
            return JavaCrashCatcherHolder.INSTANCE
        }
    }

}