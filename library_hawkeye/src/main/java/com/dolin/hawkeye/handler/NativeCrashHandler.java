package com.dolin.hawkeye.handler;

import android.app.Application;
import android.content.Context;
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

    private Context mContext = null;
    private Intent mIntent = null;
//    private IHawkeyeRemote remote = null;
//    private boolean isRegister = false;

//    private final IHawkeyeRemoteCallback mCallback = new IHawkeyeRemoteCallback.Stub() {
//        @Override
//        public void onCrashCallback(String path) throws RemoteException {
////            release();
//        }
//    };
//
//
//    private final ServiceConnection serviceConnection = new ServiceConnection() {
//        @Override
//        public void onServiceConnected(ComponentName name, IBinder service) {
//            try {
//                Log.d("dolin_hawkeye", "onServiceConnected");
//                remote = IHawkeyeRemote.Stub.asInterface(service);
//                remote.registerCallback(mCallback);
//                isRegister = true;
//            } catch (RemoteException e) {
//                e.printStackTrace();
//            }
//        }
//
//        @Override
//        public void onServiceDisconnected(ComponentName name) {
//            Log.d("dolin_hawkeye", "onServiceDisconnected");
//        }
//    };

    public static NativeCrashHandler getInstance() {
        return NativeCrashHandlerHolder.INSTANCE;
    }

    public void initialize(Application application, Map<String, String> customMap, ICrashHandler callback) {
        this.mContext = application.getApplicationContext();
        this.mIntent = new Intent(mContext, HawkeyeMonitorService.class);
        String socketName = application.getPackageName() + ".monitor";
        application.startService(mIntent);
        BoostCrashHandler.getInstance().initNativeCrashMonitor(socketName);
    }

    public void release() {
        if (mContext != null && mIntent != null) {
            mContext.stopService(mIntent);
        }
//        if (remote != null) {
//            if (mContext != null) {
//                if (isRegister) {
////                    mContext.unbindService(serviceConnection);
////                    isRegister = false;
//                }
//                if (mIntent != null) {
//                    mContext.stopService(mIntent);
//                    mIntent = null;
//                }
//                mContext = null;
//            }
//            remote = null;
//        }
    }

    private static class NativeCrashHandlerHolder {
        private static final NativeCrashHandler INSTANCE = new NativeCrashHandler();
    }
}
