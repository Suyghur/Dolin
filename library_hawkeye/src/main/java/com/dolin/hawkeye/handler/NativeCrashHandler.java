package com.dolin.hawkeye.handler;

import android.app.Application;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.IBinder;
import android.os.RemoteException;
import android.util.Log;

import com.dolin.hawkeye.HawkeyeDaemonService;
import com.dolin.hawkeye.IHaweyeDemonRemote;
import com.dolin.hawkeye.IHawkeyeDemonCallback;
import com.dolin.hawkeye.internal.ICrashHandler;

import java.util.Map;

/**
 * @author #Suyghur.
 * Created on 2021/08/06
 */
public class NativeCrashHandler {

    private IHaweyeDemonRemote remote = null;
    private IHawkeyeDemonCallback mCallback = new IHawkeyeDemonCallback.Stub() {
        @Override
        public void onCrashCallback(String logPath) throws RemoteException {

        }
    };

    private final ServiceConnection mServiceConnection = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {
            remote = IHaweyeDemonRemote.Stub.asInterface(service);
            Log.d("dolin_hawkeye", "onServiceConnected");
            try {
                remote.registerCallback(mCallback);
            } catch (RemoteException e) {
                e.printStackTrace();
            }
        }

        @Override
        public void onServiceDisconnected(ComponentName name) {

        }
    };

    public static NativeCrashHandler getInstance() {
        return NativeCrashHandlerHolder.INSTANCE;
    }

    public void initialize(Application application, Map<String, String> customMap, ICrashHandler callback) {
        Log.d("dolin_hawkeye", "NativeCrashHandler initialize");
        Intent remoteIntent = new Intent(application, HawkeyeDaemonService.class);
        application.bindService(remoteIntent, mServiceConnection, Context.BIND_AUTO_CREATE);
        BoostCrashHandler.getInstance().initNativeCrashMonitor();
    }

    private static class NativeCrashHandlerHolder {
        private static final NativeCrashHandler INSTANCE = new NativeCrashHandler();
    }
}
