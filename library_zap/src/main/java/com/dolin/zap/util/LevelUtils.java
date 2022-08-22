package com.dolin.zap.util;

import com.dolin.zap.Level;

/**
 * @author #Suyghur.
 * Created on 2021/05/19
 */
public class LevelUtils {

    public static String getLevelName(Level level) {
        return level.name();
    }

    public static String getShortLevelName(Level level) {
        String name;
        switch (level) {
            case NONE:
            case DEBUG:
                name = "D";
                break;
            case INFO:
                name = "I";
                break;
            case WARNING:
                name = "W";
                break;
            case ERROR:
                name = "E";
                break;
            default:
                if (level.ordinal() < Level.DEBUG.ordinal()) {
                    name = "D";
                } else {
                    name = "E";
                }
        }
        return name;
    }
}
