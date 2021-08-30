package com.dolin.hawkeye;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.util.Log;

import com.dolin.hawkeye.handler.BoostCrashHandler;
import com.dolin.hawkeye.utils.FileUtils;

/**
 * @author #Suyghur.
 * Created on 2021/08/23
 */
public class HawkeyeMonitorService extends Service {

    private static boolean mDaemonStarted = false;

//    private IHawkeyeRemoteCallback mCallback = null;
//
//
//    private final IHawkeyeRemote.Stub binder = new IHawkeyeRemote.Stub() {
//        @Override
//        public void registerCallback(IHawkeyeRemoteCallback callback) throws RemoteException {
//            if (mCallback == null) {
//                mCallback = callback;
//                startDaemon();
//            }
//        }
//
//        @Override
//        public void unregisterCallback() throws RemoteException {
//            if (mCallback != null) {
//                mCallback = null;
//            }
//        }
//    };

    public void startDaemon() {
        Log.d("dolin_hawkeye", "onDaemonStart");
        if (!mDaemonStarted) {
            String socketName = getPackageName() + ".monitor";
//            String logPath = getExternalFilesDir("dolin/hawkeye");
            mDaemonStarted = true;
            if (BoostCrashHandler.getInstance().zygoteNativeCrashDaemon(socketName, FileUtils.getLogFolderDir(this))) {
                Log.d("dolin_hawkeye", "unwinding daemon is started with libunwindstack.");
            } else {
                Log.e("dolin_hawkeye", "Couldn't start unwinding daemon.");
            }
        } else {
            Log.e("dolin_hawkeye", "unwinding daemon is already started.");
        }
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        startDaemon();
        return super.onStartCommand(intent, flags, startId);
    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    public void onDestroy() {
        if (mDaemonStarted) {
            mDaemonStarted = false;
            BoostCrashHandler.getInstance().releaseNativeCrashDaemon();
        }
        super.onDestroy();
    }
}
