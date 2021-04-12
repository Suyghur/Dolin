package com.suyghur.dolin

import android.app.Activity
import android.app.AlertDialog
import android.os.Bundle
import android.view.KeyEvent
import android.view.View
import android.widget.Button
import android.widget.LinearLayout
import com.suyghur.dolin.zap.Zap
import com.suyghur.dolin.zap.entity.Level
import com.suyghur.dolin.zap.util.FileUtils
import com.suyghur.dolin.zap.util.LevelUtils
import kotlin.system.exitProcess

/**
 * @author #Suyghur.
 * Created on 4/6/21
 */
class DemoActivity : Activity(), View.OnClickListener {

    private val events: MutableList<Item> = mutableListOf(
            Item(0, "Zap日志测试"),
            Item(1, "申请多个危险权限")
    )


    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        val layout = LinearLayout(this)
        layout.orientation = LinearLayout.VERTICAL
        for (event in events) {
            with(Button(this)) {
                tag = event.id
                text = event.text
                setOnClickListener(this@DemoActivity)
                layout.addView(this)
            }
        }
        setContentView(layout)
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
            }
        }
    }
}