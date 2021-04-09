package com.suyghur.dolin.simple

import android.app.Activity
import android.content.Intent
import android.os.Bundle
import android.view.View
import android.widget.Button
import android.widget.LinearLayout
import com.suyghur.dolin.zap.Zap
import com.suyghur.dolin.zap.util.FileUtils

/**
 * @author #Suyghur.
 * Created on 4/9/21
 */
class ZapActivity : Activity(), View.OnClickListener {

    private val events: MutableList<Item> = mutableListOf(
            Item(0, "DEBUG日志"),
            Item(1, "INFO日志"),
            Item(2, "WARNING日志"),
            Item(3, "ERROR日志"),
    )

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        val layout = LinearLayout(this)
        layout.orientation = LinearLayout.VERTICAL
        for (event in events) {
            with(Button(this)) {
                tag = event.id
                text = event.text
                setOnClickListener(this@ZapActivity)
                layout.addView(this)
            }
        }
        setContentView(layout)
        Zap.d(FileUtils.getLogDir(this))
    }

    override fun onClick(v: View?) {
    }

    companion object {
        fun start(activity: Activity) {
            activity.startActivity(Intent(activity, ZapActivity::class.java).addFlags(Intent.FLAG_ACTIVITY_NEW_TASK))
        }
    }
}