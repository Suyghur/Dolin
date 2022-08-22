package com.dolin.zap.impl;

import com.dolin.zap.Level;

/**
 * @author #Suyghur.
 * Created on 2021/05/19
 */
public interface IFormatter {

    String format(Level level, String tag, String msg);
}
