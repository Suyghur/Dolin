// IHaweyeDemonRemote.aidl
package com.dolin.hawkeye;
import com.dolin.hawkeye.IHawkeyeDemonCallback;

// Declare any non-default types here with import statements

interface IHaweyeDemonRemote {
    /**
     * Demonstrates some basic types that you can use as parameters
     * and return values in AIDL.
     */
    void registerCallback(IHawkeyeDemonCallback callback);

    void unregisterCallback();

}