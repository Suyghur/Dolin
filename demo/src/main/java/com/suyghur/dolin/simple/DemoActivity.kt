package com.suyghur.dolin.simple

import android.app.Activity
import android.os.Bundle
import android.view.View
import android.widget.Button
import android.widget.LinearLayout
import com.suyghur.dolin.zap.Zap
import com.suyghur.dolin.zap.entity.Level
import com.suyghur.dolin.zap.util.LevelUtils

/**
 * @author #Suyghur.
 * Created on 4/6/21
 */
class DemoActivity : Activity(), View.OnClickListener {

    private val events: MutableList<Item> = mutableListOf(
            Item(0, "申请单个危险权限"),
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

    override fun onClick(v: View?) {
        v?.apply {

        }
    }
}