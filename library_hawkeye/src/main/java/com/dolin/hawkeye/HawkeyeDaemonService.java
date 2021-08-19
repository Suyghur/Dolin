package com.dolin.hawkeye;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.os.RemoteException;
import android.util.Log;

/**
 * @author #Suyghur.
 * Created on 2021/08/18
 */
public class HawkeyeDaemonService extends Service {

    private static final String DAEMON_NAME = "HawkeyeDaemon";

    private static boolean mDaemonStarted = false;

    private static IHawkeyeDemonCallback mCallback = null;

    private final IHaweyeDemonRemote.Stub binder = new IHaweyeDemonRemote.Stub() {
        @Override
        public void registerCallback(IHawkeyeDemonCallback callback) throws RemoteException {
            if (mCallback == null) {
                Log.d("dolin_hawkeye", "registerCallback");
                mCallback = callback;
            }
        }

        @Override
        public void unregisterCallback() throws RemoteException {
            if (mCallback != null) {
                mCallback = null;
            }
        }
    };

    @Override
    public void onCreate() {
        super.onCreate();
    }


    @Override
    public void onDestroy() {
        super.onDestroy();
    }

    @Override
    public IBinder onBind(Intent intent) {
        Log.d("dolin_hawkeye", "onBind");
        return binder;
    }

}
