# Add project specific ProGuard rules here.
# You can control the set of applied configuration files using the
# proguardFiles setting in build.gradle.
#
# For more details, see
#   http://developer.android.com/guide/developing/tools/proguard.html

# If your project uses WebView with JS, uncomment the following
# and specify the fully qualified class name to the JavaScript interface
# class:
#-keepclassmembers class fqcn.of.javascript.interface.for.webview {
#   public *;
#}

# Uncomment this to preserve the line number information for
# debugging stack traces.
#-keepattributes SourceFile,LineNumberTable

# If you keep the line number information, uncomment this to
# hide the original source file name.
#-renamesourcefileattribute SourceFile

# 代码混淆压缩比，在0~7之间，默认为5，一般不做修改
-optimizationpasses 7
# 混合时不使用大小写混合，混合后的类名为小写
-dontusemixedcaseclassnames
# 指定不去忽略非公共库的类
-dontskipnonpubliclibraryclasses
-dontoptimize
# 这句话能够使我们的项目混淆后产生映射文件
# 包含有类名->混淆后类名的映射关系
-verbose
-ignorewarnings
# 指定不去忽略非公共库的类成员
-dontskipnonpubliclibraryclassmembers
# 指定混淆是采用的算法，后面的参数是一个过滤器
# 这个过滤器是谷歌推荐的算法，一般不做更改
-optimizations !code/simplification/arithmetic,!field/*,!class/merging/*

# 保留java与js交互注解
-keepattributes *Annotation*
-keepattributes *JavascriptInterface*

# keep javascript注释的方法，使用到webview js回调方法的需要添加此配置
-keepclassmembers class * {
    @android.webkit.JavascriptInterface <methods>;
}

#  #保留内部接口或内部类、内部类、泛型签名类型
-keepattributes Exceptions,InnerClasses,Signature

# 将崩溃日志文件来源重命名为"SourceFile"
-renamesourcefileattribute SourceFile
# 产生有用的混淆堆栈跟踪
-keepattributes SourceFile,LineNumberTable
# 保留注释
-keepattributes *Annotation*

# 保留函数的参数名
-keepparameternames

-keepnames class * implements java.io.Serializable
-keepclassmembers class * implements java.io.Serializable {
   static final long serialVersionUID;
   private static final java.io.ObjectStreamField[] serialPersistentFields;
   !static !transient <fields>;
   private void writeObject(java.io.ObjectOutputStream);
   private void readObject(java.io.ObjectInputStream);
   java.lang.Object writeReplace();
   java.lang.Object readResolve();
}

-keepclassmembers class **.R$* {
    public static <fields>;
}
-keep class **.R$* {
 *;
}

-keep public class * extends android.app.Activity{
	public <fields>;
	public <methods>;
}
-keep public class * extends android.app.Application{
	public <fields>;
	public <methods>;
}
-keep public class * extends android.app.Service
-keep public class * extends android.content.BroadcastReceiver
-keep public class * extends android.content.ContentProvider
-keep public class * extends android.app.backup.BackupAgentHelper
-keep public class * extends android.preference.Preference


-keepclassmembers enum * {
    public static **[] values();
    public static ** valueOf(java.lang.String);
}

-keepclasseswithmembers class * {
	public <init>(android.content.Context, android.util.AttributeSet);
}

-keepclasseswithmembers class * {
	public <init>(android.content.Context, android.util.AttributeSet, int);
}

-keepclasseswithmembernames class *{
	native <methods>;
}

-keep class * implements android.os.Parcelable {
  public static final android.os.Parcelable$Creator *;
}

-keepclasseswithmembers class * {
    ... *JNI*(...);
}

-keepclasseswithmembernames class * {
	... *JRI*(...);
}

-keep class **JNI* {*;}

# 保留native方法
-keepclasseswithmembernames class * {
    native <methods>;
}

# keep R文件的静态字段
-keepclassmembers class **.R$* {
    public static <fields>;
}

-keeppackagenames com.dolin.zap.**
-keep class com.dolin.zap.Version{public <fields>;public <methods>;}
-keep class com.dolin.zap.Zap{public <fields>;public <methods>;}
-keep class com.dolin.zap.impl.Record2MMap{*;}
-keep class com.dolin.zap.entity.Level{*;}
-keep class com.dolin.zap.entity.Config{public <fields>;public <methods>;}
-keep class com.dolin.zap.entity.Config$Builder{public <fields>;public <methods>;}