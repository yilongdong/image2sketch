package com.scut.example

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.os.Environment
import android.provider.MediaStore
import android.widget.Toast
import androidx.core.content.contentValuesOf
import com.scut.example.databinding.ActivityShowBinding
import java.io.BufferedInputStream
import java.io.File
import java.io.FileInputStream


class ShowActivity : AppCompatActivity() {
    private lateinit var binding: ActivityShowBinding

    private lateinit var fileDir: File;

    private lateinit var originImage: Bitmap
    private lateinit var sketchImage: Bitmap

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityShowBinding.inflate(layoutInflater)
        setContentView(binding.root)

        fileDir = getExternalFilesDir(Environment.DIRECTORY_PICTURES)!!

        // 获取上一个activity传过来的文件名
        val fileName = intent.getStringExtra("fileName")!!

        // 读取两张图片
        BufferedInputStream(FileInputStream(File(fileDir, fileName))).use {
            sketchImage = BitmapFactory.decodeStream(it)
        }
        BufferedInputStream(FileInputStream(File(fileDir, "origin-${fileName}"))).use {
            originImage = BitmapFactory.decodeStream(it)
        }

        binding.imageView.setImageBitmap(sketchImage);

        binding.apply {
            // 暴力交换
            swapBtn.setOnClickListener {
                val temp = originImage
                originImage = sketchImage
                sketchImage = temp
                binding.imageView.setImageBitmap(sketchImage)
            }

            // 把图片保存到系统相册
            downloadBtn.setOnClickListener {
                val contentValues = contentValuesOf(
                    MediaStore.Images.ImageColumns.RELATIVE_PATH to Environment.DIRECTORY_PICTURES,
                    MediaStore.Images.ImageColumns.DISPLAY_NAME to fileName.substring(0, fileName.length - 4),
                    MediaStore.Images.ImageColumns.MIME_TYPE to "image/jpeg",
                    MediaStore.Images.ImageColumns.WIDTH to sketchImage.width,
                    MediaStore.Images.ImageColumns.HEIGHT to sketchImage.height
                )
                val uri = contentResolver.insert(MediaStore.Images.Media.EXTERNAL_CONTENT_URI, contentValues)!!
                contentResolver.openOutputStream(uri)!!.use {
                    sketchImage.compress(Bitmap.CompressFormat.JPEG, 100, it)
                    it.flush()
                }
                Toast.makeText(this@ShowActivity, "下载成功！已保存到系统相册", Toast.LENGTH_LONG).show()
            }

            returnBtn.setOnClickListener {
                finish()
            }
        }
    }

}