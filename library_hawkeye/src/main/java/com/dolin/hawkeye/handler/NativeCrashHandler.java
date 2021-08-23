package com.dolin.hawkeye.handler;

import android.app.Application;
import android.content.Intent;
import android.util.Log;

import com.dolin.hawkeye.HawkeyeMonitorService;
import com.dolin.hawkeye.internal.ICrashHandler;

import java.util.Map;

/**
 * @author #Suyghur.
 * Created on 2021/08/06
 */
public class NativeCrashHandler {

//    private IHaweyeDemonRemote remote = null;
//    private IHawkeyeDemonCallback mCallback = new IHawkeyeDemonCallback.Stub() {
//        @Override
//        public void onCrashCallback(String logPath) throws RemoteException {
//
//        }
//    };
//
//    private final ServiceConnection mServiceConnection = new ServiceConnection() {
//        @Override
//        public void onServiceConnected(ComponentName name, IBinder service) {
//            remote = IHaweyeDemonRemote.Stub.asInterface(service);
//            Log.d("dolin_hawkeye", "onServiceConnected");
//            try {
//                remote.registerCallback(mCallback);
//                BoostCrashHandler.getInstance().initNativeCrashMonitor(socketName);
//            } catch (RemoteException e) {
//                e.printStackTrace();
//            }
//        }
//
//        @Override
//        public void onServiceDisconnected(ComponentName name) {
//
//        }
//    };

    public static NativeCrashHandler getInstance() {
        return NativeCrashHandlerHolder.INSTANCE;
    }

    public void initialize(Application application, Map<String, String> customMap, ICrashHandler callback) {
        Log.d("dolin_hawkeye", "NativeCrashHandler initialize");
        Intent intent = new Intent(application, HawkeyeMonitorService.class);
        try {
            application.startService(intent);
        } catch (RuntimeException e) {
            e.printStackTrace();
        }
       BoostCrashHandler.getInstance().initNativeCrashMonitor(application.getPackageName()+".hawkeyedaemon");
    }

    private static class NativeCrashHandlerHolder {
        private static final NativeCrashHandler INSTANCE = new NativeCrashHandler();
    }
}
