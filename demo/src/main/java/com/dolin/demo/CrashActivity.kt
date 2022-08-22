//package com.dolin.demo
//
//import android.app.Activity
//import android.content.Intent
//import android.os.Bundle
//import android.view.View
//import android.widget.Button
//import android.widget.LinearLayout
//import com.dolin.hawkeye.Hawkeye
//
///**
// * @author #Suyghur.
// * Created on 2021/08/18
// */
//class CrashActivity : Activity(), View.OnClickListener {
//
//    private val events: MutableList<Item> = mutableListOf(
//        Item(0, "抛出一个Java层主线程崩溃"),
//        Item(1, "抛出一个Java层子线程崩溃"),
//        Item(2, "抛出一个Native层崩溃")
//    )
//
//    override fun onCreate(savedInstanceState: Bundle?) {
//        super.onCreate(savedInstanceState)
//        val layout = LinearLayout(this)
//        layout.orientation = LinearLayout.VERTICAL
//        for (event in events) {
//            with(Button(this)) {
//                isAllCaps = false
//                tag = event.id
//                text = event.text
//                setOnClickListener(this@CrashActivity)
//                layout.addView(this)
//            }
//        }
//        setContentView(layout)
//
//        Hawkeye.getInstance().setCustomKV("device_id", "my_device_id_123")
//        Hawkeye.getInstance().setCustomKV("user_id", "my_user_id_456")
//    }
//
//    override fun onResume() {
//        super.onResume()
//    }
//
//    override fun onDestroy() {
//        super.onDestroy()
//    }
//
//    override fun onClick(v: View?) {
//        v?.apply {
//            when (tag as Int) {
//                0 -> Hawkeye.getInstance().testJavaCrash(false)
//                1 -> Hawkeye.getInstance().testJavaCrash(true)
//                2 -> Hawkeye.getInstance().testNativeCrash()
//            }
//        }
//    }
//
//    companion object {
//        fun start(activity: Activity) {
//            activity.startActivity(Intent(activity, CrashActivity::class.java).addFlags(Intent.FLAG_ACTIVITY_NEW_TASK))
//        }
//    }
//}