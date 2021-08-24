// IHawkeyeRemote.aidl
package com.dolin.hawkeye;
import com.dolin.hawkeye.IHawkeyeRemoteCallback;

// Declare any non-default types here with import statements

interface IHawkeyeRemote {
    /**
     * Demonstrates some basic types that you can use as parameters
     * and return values in AIDL.
     */
    void registerCallback(IHawkeyeRemoteCallback callback);

    void unregisterCallback();
}