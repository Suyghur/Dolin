package com.dolin.crashlytics.utils;

import java.io.BufferedReader;
import java.io.FileReader;

/**
 * @author #Suyghur.
 * Created on 2021/08/02
 */
public class FileUtils {

    public static String getFileContent(String pathName, int limit) {
        StringBuilder sb = new StringBuilder();
        BufferedReader br = null;
        String line;
        int n = 0;
        try {
            br = new BufferedReader(new FileReader(pathName));
            while (null != (line = br.readLine())) {
                String p = line.trim();
                if (p.length() > 0) {
                    n++;
                    if (limit == 0 || n <= limit) {
                        sb.append("  ").append(p).append("\n");
                    }
                }
            }
            if (limit > 0 && n > limit) {
                sb.append("  ......\n").append("  (number of records: ").append(n).append(")\n");
            }
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            if (br != null) {
                try {
                    br.close();
                } catch (Exception ignored) {
                }
            }
        }
        return sb.toString();
    }
}
