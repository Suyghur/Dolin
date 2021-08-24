// IHawkeyeRemoteCallback.aidl
package com.dolin.hawkeye;

// Declare any non-default types here with import statements

interface IHawkeyeRemoteCallback {
    /**
     * Demonstrates some basic types that you can use as parameters
     * and return values in AIDL.
     */
    void onCrashCallback(String path);
}