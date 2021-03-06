package com.dolin.demo

import android.app.Activity
import android.content.Intent
import android.os.Bundle
import android.view.View
import android.widget.Button
import android.widget.LinearLayout
import com.dolin.zap.Zap

/**
 * @author #Suyghur.
 * Created on 4/9/21
 */
class ZapActivity : Activity(), View.OnClickListener {

    private val events: MutableList<Item> = mutableListOf(
        Item(0, "DEBUG日志"),
        Item(1, "INFO日志"),
        Item(2, "WARNING日志"),
        Item(3, "ERROR日志")
    )

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        val layout = LinearLayout(this)
        layout.orientation = LinearLayout.VERTICAL
        for (event in events) {
            with(Button(this)) {
                isAllCaps = false
                tag = event.id
                text = event.text
                setOnClickListener(this@ZapActivity)
                layout.addView(this)
            }
        }
        setContentView(layout)
    }

    override fun onResume() {
        super.onResume()
    }

    override fun onDestroy() {
        super.onDestroy()
    }

    override fun onClick(v: View?) {
        v?.apply {
            when (tag as Int) {
                0 -> Zap.d("测试DEBUG日志")
                1 -> Zap.i("测试INFO日志")
                2 -> Zap.w("测试WARNING日志")
                3 -> Zap.e("测试ERROR日志")
            }
        }
    }

    companion object {
        fun start(activity: Activity) {
            activity.startActivity(Intent(activity, ZapActivity::class.java).addFlags(Intent.FLAG_ACTIVITY_NEW_TASK))
        }
    }
}