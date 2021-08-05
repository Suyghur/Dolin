package com.dolin.comm.util;

import android.content.Context;
import android.text.TextUtils;

import java.io.File;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;

/**
 * @author #Suyghur.
 * Created on 2021/05/19
 */
public class LogFileUtils {


    public static void cleanOverdueLog(Context context, long overdueDayMs) {
        File folder = new File(getLogFolderDir(context));
        SimpleDateFormat format = new SimpleDateFormat("yyyy-MM-dd", Locale.getDefault());
        File[] files = folder.listFiles();
        if (files != null) {
            for (File f : files) {
                if (f.isDirectory() && isFileOverdue(f, overdueDayMs, format)) {
                    deleteDirectory(f.getPath());
                }
            }
        }
    }

//    public static void cleanOverdueLog(String folderPath, long overdueDayMs) {
//        File folder = new File(folderPath);
//        if (!folder.exists()) {
//            return;
//        }
//        File[] files = folder.listFiles();
//        if (files != null) {
//            for (File f : files) {
//                if (f.isDirectory()) {
//                    cleanOverdueLog(folderPath, overdueDayMs);
//                } else {
//                    if (isFileOverdue(f, overdueDayMs)) {
//                        f.delete();
//                    }
//                }
//            }
//        }
//    }

    private static boolean isFileOverdue(File file, long overdueDayMs) {
        return (System.currentTimeMillis() - file.lastModified()) > overdueDayMs;
    }

    private static boolean isFileOverdue(File file, long overdueDayMs, SimpleDateFormat format) {
        String time = file.getName();
        try {
            Date date = format.parse(time);
            if (date != null) {
                long ts = date.getTime();
                return (System.currentTimeMillis() - ts) > overdueDayMs;
            }
        } catch (ParseException e) {
            e.printStackTrace();
        }
        return false;
    }

    public static String getLogFolderDir(Context context) {
        File folder = context.getExternalFilesDir("dolin/zap");
        if (folder == null) {
            folder = new File(context.getFilesDir(), "dolin/zap");
        }
        if (!folder.exists()) {
            folder.mkdirs();
        }
        return folder.getAbsolutePath();
    }

    public static String getLogDir(String folderPath, String date) {
        File folder = new File(folderPath + File.separator + date);
        if (!folder.exists()) {
            folder.mkdirs();
        }
        return folder.getAbsolutePath();
    }

    public static int getLogFileNumByDate(String folderPath, String date) {
        int num = 0;
        File folder = new File(folderPath);
        if (!folder.exists()) {
            return num;
        }
        File[] files = folder.listFiles();
        if (files == null) {
            return num;
        } else {
            return files.length;
        }
    }

    public static boolean deleteFile(String path) {
        if (TextUtils.isEmpty(path)) {
            return false;
        }
        File file = new File(path);
        if (file.exists()) {
            return file.delete();
        }
        return false;
    }

    /**
     * 删除文件夹以及所有子目录和文件
     */
    public static boolean deleteDirectory(String path) {
        if (TextUtils.isEmpty(path)) {
            return false;
        }
        boolean flag;
        // 如果path不以文件分隔符结尾，自动添加文件分隔符
        if (!path.endsWith(File.separator)) {
            path = path + File.separator;
        }
        File dirFile = new File(path);
        // 如果dir对应的文件不存在，或者不是一个目录，则退出
        if (!dirFile.exists() || !dirFile.isDirectory()) {
            return false;
        }
        flag = true;
        // 删除文件夹下的所有文件(包括子目录)
        File[] files = dirFile.listFiles();
        if (files != null && files.length > 0) {
            for (File file : files) {
                if (file.isFile()) {
                    // 删除子文件
                    flag = deleteFile(file.getAbsolutePath());
                } else {
                    // 删除子目录
                    flag = deleteDirectory(file.getAbsolutePath());
                }
                if (!flag) {
                    break;
                }
            }
        }
        if (!flag) {
            return false;
        }
        // 删除当前目录
        return dirFile.delete();
    }

}
