package com.ihuntto.libpng

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.graphics.Color
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import com.ihuntto.libpng.databinding.ActivityMainBinding
import kotlin.random.Random
import androidx.core.graphics.createBitmap
import androidx.core.graphics.set
import kotlinx.coroutines.DelicateCoroutinesApi
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.GlobalScope
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
import java.io.BufferedOutputStream
import java.io.File
import java.io.FileOutputStream
import java.io.IOException

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding

    @OptIn(DelicateCoroutinesApi::class)
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        binding.testBtn.setOnClickListener {
            binding.testTips.setText(R.string.testing)
            binding.testBtn.isEnabled = false
            GlobalScope.launch(Dispatchers.IO) {
                var time = System.currentTimeMillis()
                val bitmap = createColorNoiseBitmap(binding.root.width, binding.root.height)
                val sb = StringBuilder()
                sb.append("create bitmap used ${System.currentTimeMillis() - time}ms\n")
                time = System.currentTimeMillis()
                externalCacheDir?.absolutePath?.let { cacheDir ->
                    try {
                        BufferedOutputStream(FileOutputStream(cacheDir + File.separatorChar + "noise1.png")).use {
                            bitmap.compress(Bitmap.CompressFormat.PNG, 100, it)
                        }
                        sb.append("bitmap compress used ${System.currentTimeMillis() - time}ms\n")
                        time = System.currentTimeMillis()
                        PNG.save(bitmap, cacheDir + File.separatorChar + "noise0.png")
                        sb.append("libpng save used ${System.currentTimeMillis() - time}ms\n")
                    } catch (e: IOException) {
                        e.printStackTrace()
                    }
                }
                withContext(Dispatchers.Main) {
                    binding.testTips.text = sb.toString()
                    binding.testBtn.isEnabled = true
                }
            }
        }


    }

    fun createColorNoiseBitmap(width: Int, height: Int): Bitmap {
        val bitmap = createBitmap(width, height)
        val random = Random(System.currentTimeMillis())

        for (x in 0 until width) {
            for (y in 0 until height) {
                val r = random.nextInt(256)
                val g = random.nextInt(256)
                val b = random.nextInt(256)
                val color = Color.argb(255, r, g, b)
                bitmap[x, y] = color
            }
        }

        return bitmap
    }
}