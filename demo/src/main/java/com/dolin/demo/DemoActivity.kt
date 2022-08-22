package com.dolin.demo

import android.app.Activity
import android.app.AlertDialog
import android.os.Bundle
import android.view.KeyEvent
import android.view.View
import android.widget.Button
import android.widget.LinearLayout
import android.widget.TextView
import com.dolin.zap.Zap
import kotlin.system.exitProcess

/**
 * @author #Suyghur.
 * Created on 4/6/21
 */
class DemoActivity : Activity(), View.OnClickListener {

    private lateinit var textView: TextView

    private val events: MutableList<Item> = mutableListOf(
        Item(0, "Zap日志测试"),
//        Item(1, "Hawkeye崩溃测试"),
//        Item(2, "Hawkeye崩溃测试")
    )


    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        initView()
        initDeviceInfo()
    }

    private fun initView() {
        val layout = LinearLayout(this)
        layout.orientation = LinearLayout.VERTICAL
        textView = TextView(this)
        layout.addView(textView)
        for (event in events) {
            with(Button(this)) {
                isAllCaps = false
                text = event.text
                tag = event.id
                setOnClickListener(this@DemoActivity)
                layout.addView(this)
            }
        }
        setContentView(layout)
    }

    private fun initDeviceInfo() {
        val sb = StringBuilder()
//        sb.append("Android ID：").append(DeviceInfoUtils.getAndroidDeviceId(this)).append("\n")
//        sb.append("厂商：").append(DeviceInfoUtils.getMobileBrand()).append("\n")
//        sb.append("型号：").append(DeviceInfoUtils.getDeviceModel()).append("\n")
//        sb.append("系统版本：").append(DeviceInfoUtils.getDeviceSoftWareVersion()).append("\n")
//        sb.append("cpu核数：").append(DeviceInfoUtils.getCpuCount()).append("\n")
//        sb.append("cpu架构：").append(DeviceInfoUtils.getCpuAbi()).append("\n")
//        sb.append("本机内存：").append(DeviceInfoUtils.getRAM()).append("\n")
//        sb.append("本机剩余内存：").append(DeviceInfoUtils.getAvailMem(this)).append("M\n")
//        sb.append("系统SDK版本：${Build.VERSION.SDK_INT}")
        textView.text = sb.toString()
    }

    override fun onKeyDown(keyCode: Int, event: KeyEvent?): Boolean {
        if (keyCode == KeyEvent.KEYCODE_BACK) {
            AlertDialog.Builder(this)
                .setTitle("退出应用")
                .setCancelable(false)
                .setMessage("是否退出")
                .setPositiveButton("确认") { dialog, _ ->
                    dialog?.dismiss()
                    finish()
                }
                .setNegativeButton("取消") { dialog, _ ->
                    dialog?.dismiss()
                }.show()
            return true
        }
        return super.onKeyDown(keyCode, event)
    }

    override fun onDestroy() {
        super.onDestroy()
        Zap.recycle()
        exitProcess(0)
    }

    override fun onClick(v: View?) {
        v?.apply {
            when (tag as Int) {
                0 -> ZapActivity.start(this@DemoActivity)
//                1 -> CrashActivity.start(this@DemoActivity)
//                2 -> Hawkeye.getInstance().testNativeCrash()
            }
        }
    }
}