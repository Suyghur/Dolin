// IHawkeyeDemonCallback.aidl
package com.dolin.hawkeye;

// Declare any non-default types here with import statements

interface IHawkeyeDemonCallback {
    /**
     * Demonstrates some basic types that you can use as parameters
     * and return values in AIDL.
     */
    void onCrashCallback(String logPath);
}