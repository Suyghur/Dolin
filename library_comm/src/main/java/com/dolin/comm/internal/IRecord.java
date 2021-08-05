package com.dolin.comm.internal;

/**
 * @author #Suyghur.
 * Created on 2021/05/19
 */
public interface IRecord {

    void write(String msg);

    void asyncFlush();

    void expLogFile(String path, int limitSize);

    void release();
}
