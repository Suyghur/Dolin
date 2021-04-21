# Zap

> 基于 mmap 的 Android 日志收集框架

## 使用

- config配置类

|	属性	|	默认值	|	说明	|	Builder中方法	|
| :--: | :--: | :--: | :--: |
|	folderDir	|	应用私有目录/dolin/zap	|	缓存日志文件的根目录	| setFolderDir() |
|	tag	|	dolin_zap	|日志TAG标签|setLogTag()|
|	logcatLevel	|	Level.DEBUG	|	Logcat中打印最低等级	| setLogcatLevel() |
|	recordLevel	|	Level.DEBUG	|	缓存日志的最低等级	| setRecordLevel() |
|	recordEnable	|	true	|	是否启用缓存日志	| setRecordEnable() |
|	compressEnable	|	true	|	缓存文件内容是否压缩	| setRecordCompressEnable() |
|	overdueDay	|	3	|	日志文件过期天数，单位天	| setOverdueDay() |
|	fileSizeLimitDay	|	15	|	单天日志文件大小上限，单位M	| setFileSizeLimitDay() |

-  在Application#onCreate()中进行初始化

```kotlin
override fun onCreate() {
	super.onCreate()
	val config = Config.Builder()
                //logcat输出最低等级
                .setLogcatLevel(Level.DEBUG)
                //是否开启缓存日志
                .setRecordEnable(true)
                //缓存日志最低等级
                .setRecordLevel(Level.DEBUG)
                //是否开启压缩缓存日志内容
                .setRecordCompressEnable(true)
                //缓存文件的过期时间
                .setOverdueDay(3)
                //缓存文件大小限制，超过则会自动扩容新文件
                .setFileSizeLimitDay(15)
                //缓存文件内容是否压缩
                .setRecordCompressEnable(false)
                .create()
                Zap.initialize(this, config)
}
```

- 打印

```kotlin
	//接收任何数据类型，如果是数组，内部会进行处理
	Zap.d("测试DEBUG日志")
	Zap.i("测试INFO日志")
	Zap.w("测试WARNING日志")
	Zap.e("测试ERROR日志")
```

- 释放Zap组件

```kotlin
override fun onDestroy() {
	super.onDestroy()
  ···
	Zap.recycle()
  ···
}
```

## 更多信息

[存储逻辑](STORAGE_INFO.md)

[缓存Buffer结构](BUFFER_INFO.md)

## Thanks

- [Tencent/mars](https://github.com/Tencent/mars)