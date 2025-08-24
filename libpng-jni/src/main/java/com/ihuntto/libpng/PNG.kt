package com.ihuntto.libpng

import android.graphics.Bitmap

class PNG {

    companion object {
        init {
            System.loadLibrary("png-jni")
        }

        external fun save(bitmap: Bitmap, filepath: String): Boolean
    }
}