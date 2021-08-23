package com.dolin.hawkeye;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.util.Log;

import androidx.annotation.Nullable;

import com.dolin.hawkeye.handler.BoostCrashHandler;

import java.io.File;

/**
 * @author #Suyghur.
 * Created on 2021/08/23
 */
public class HawkeyeMonitorService extends Service {

    private static boolean mDaemonStarted = false;

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        Log.d("dolin_hawkeye", "onStartCommand");
        if (!mDaemonStarted) {
            String socketName = getPackageName() + ".hawkeyedaemon";
            String logPath = getExternalFilesDir("dolin/hawkeye") + File.separator + "test.hawkeye";
            mDaemonStarted = true;
            if (BoostCrashHandler.getInstance().initNativeCrashDaemon(socketName, logPath)) {
                Log.d("dolin_hawkeye", "unwinding daemon is started with libunwindstack, log path: " + logPath);
                onDaemonStart(logPath);
            } else {
                Log.e("dolin_hawkeye", "Couldn't start unwinding daemon.");
            }
        } else {
            Log.e("dolin_hawkeye", "unwinding daemon is already started.");
        }
        return Service.START_STICKY;
    }

    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    public void onDestroy() {
        if (mDaemonStarted) {
            mDaemonStarted = false;
        }
        super.onDestroy();
    }

    private void onDaemonStart(String logPath) {
        Log.d("dolin_hawkeye", "onDaemonStart");
    }
}
