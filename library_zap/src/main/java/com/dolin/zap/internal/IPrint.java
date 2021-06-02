package com.dolin.zap.internal;

/**
 * @author #Suyghur.
 * Created on 2021/05/19
 */
public interface IPrint {

    void d(Object obj);

    void d(String tag, Object obj);

    void i(Object obj);

    void i(String tag, Object obj);

    void w(Object obj);

    void w(String tag, Object obj);

    void e(Object obj);

    void e(String tag, Object obj);

}
