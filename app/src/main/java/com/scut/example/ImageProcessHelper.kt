package com.scut.example

import android.graphics.Bitmap
import android.graphics.BitmapFactory

object ImageProcessHelper {
    private var texture: Bitmap;
    init {
        System.loadLibrary("example")
        texture = BitmapFactory.decodeStream(
            ImageApplication.context.resources.assets.open("texture.png"))
    }
    fun imageProcessHelper(src: Bitmap): Bitmap {
        val result = Bitmap.createBitmap(src.width, src.height, Bitmap.Config.ARGB_8888)

        // 转换失败直接抛异常
        if (!imageProcess(src, texture, result)) {
            throw RuntimeException()
        }
        return result
    }
    external fun imageProcess(src: Bitmap, pattern: Bitmap, result: Bitmap): Boolean
}