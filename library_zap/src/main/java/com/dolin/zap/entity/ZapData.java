package com.dolin.zap.entity;

/**
 * @author #Suyghur.
 * Created on 2021/05/19
 */
public class ZapData {

    private static final int MAX_POOL_SIZE = 50;
    private static final Object sPoolSync = new Object();
    private static ZapData sPool = null;
    public static int sPoolSize = 0;


    public Level level = Level.NONE;
    public String tag = "";
    public String msg = "";
    private ZapData next = null;

    public static ZapData obtain() {
        synchronized (sPoolSync) {
            if (sPool != null) {
                ZapData data = sPool;
                sPool = data.next;
                data.next = null;
                sPoolSize--;
                return data;
            }
        }
        return new ZapData();
    }

    public static ZapData obtain(Level level, String tag, String msg) {
        ZapData data = obtain();
        data.level = level;
        data.tag = tag;
        data.msg = msg;
        return data;
    }

    public void recycle() {
        this.level = Level.NONE;
        this.tag = "";
        this.msg = "";
        synchronized (sPoolSync) {
            if (sPoolSize < MAX_POOL_SIZE) {
                next = sPool;
                sPool = this;
                sPoolSize++;
            }
        }
    }

    @Override
    public String toString() {
        return "ZapData{" +
                "level=" + level +
                ", tag='" + tag + '\'' +
                ", msg='" + msg + '\'' +
                ", next=" + next +
                '}';
    }
}
