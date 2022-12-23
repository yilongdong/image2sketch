package com.scut.example

import android.graphics.Bitmap
import org.opencv.core.Mat

object ImageAddress {
    init {
        System.loadLibrary("example")
    }
    external fun imageProcess(src: Bitmap, pattern: Bitmap, result: Bitmap)
}