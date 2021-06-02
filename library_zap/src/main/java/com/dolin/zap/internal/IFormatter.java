package com.dolin.zap.internal;

import com.dolin.zap.entity.Level;

/**
 * @author #Suyghur.
 * Created on 2021/05/19
 */
public interface IFormatter {

    String format(Level level, String tag, String msg);
}
