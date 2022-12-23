package com.scut.example

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import com.scut.example.databinding.ActivityShowBinding


class ShowActivity : AppCompatActivity() {
    private lateinit var binding: ActivityShowBinding

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityShowBinding.inflate(layoutInflater)
        setContentView(binding.root)

        val texture = BitmapFactory.decodeStream(resources.assets.open("texture.png"))
        val testImage = BitmapFactory.decodeStream(resources.assets.open("test_image.jpeg"))
        val result = Bitmap.createBitmap(testImage);

        ImageAddress.imageProcess(testImage, texture, result);

        binding.imageView.setImageBitmap(result);
    }


}