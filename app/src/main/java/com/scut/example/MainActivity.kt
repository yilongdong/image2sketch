package com.scut.example

import android.app.Activity
import android.content.Intent
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.os.Environment
import android.util.Log
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.EditText
import android.widget.ImageView
import android.widget.SearchView
import android.widget.TextView
import android.widget.Toast
import androidx.activity.result.contract.ActivityResultContracts
import androidx.appcompat.app.AlertDialog
import androidx.recyclerview.widget.LinearLayoutManager
import androidx.recyclerview.widget.RecyclerView
import com.scut.example.databinding.ActivityMainBinding
import java.io.BufferedInputStream
import java.io.BufferedOutputStream
import java.io.File
import java.io.FileInputStream
import java.io.FileOutputStream
import java.sql.Timestamp
import java.util.UUID
import kotlin.concurrent.thread

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding

    private val imageList = ArrayList<ImageContent>()
    private lateinit var imageAdapter: ImageAdapter

    private val imageDao = ImageDatabase.getDatabase().imageDao()

    private lateinit var fileDir: File
    private var tempName = "";

    private val resultHandler =
        registerForActivityResult(ActivityResultContracts.StartActivityForResult()) { result ->
            if (result.resultCode == Activity.RESULT_OK) {
                result.data?.data?.let { uri ->
                    val origin = contentResolver.openFileDescriptor(uri, "r")!!.use {
                        BitmapFactory.decodeFileDescriptor(it.fileDescriptor)
                    }

                    // 转换
                    val sketch: Bitmap
                    try {
                        sketch = ImageProcessHelper.imageProcessHelper(origin)
                    }
                    catch (e: RuntimeException) {
                        Toast.makeText(this@MainActivity, "转换失败！", Toast.LENGTH_LONG).show()
                        return@registerForActivityResult
                    }

                    // 保存文件
                    val fileName = "${UUID.randomUUID()}.jpg"
                    BufferedOutputStream(FileOutputStream(File(fileDir, "origin-${fileName}")))
                        .use {
                            origin.compress(Bitmap.CompressFormat.JPEG, 100, it)
                            it.flush()
                        }
                    BufferedOutputStream(FileOutputStream(File(fileDir, fileName)))
                        .use {
                            sketch.compress(Bitmap.CompressFormat.JPEG, 100, it)
                            it.flush()
                        }

                    // 保存到数据库并更新列表
                    thread {
                        val createTime = Timestamp(System.currentTimeMillis()).toString()
                        val id = imageDao.insertItem(ImageEntity(fileName, tempName, createTime))
                        runOnUiThread {
                            imageAdapter.addItem(ImageContent(id, tempName, createTime, fileName, sketch))
                            Toast.makeText(this@MainActivity, "生成成功！", Toast.LENGTH_LONG).show()
                        }
                    }
                }
            }
        }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        // 初始化列表
        initList(null)
        val layoutManager = LinearLayoutManager(this)
        binding.recyclerView.layoutManager = layoutManager
        imageAdapter = ImageAdapter()
        binding.recyclerView.adapter = imageAdapter

        fileDir = getExternalFilesDir(Environment.DIRECTORY_PICTURES)!!

        binding.apply {
            chooseBtn.setOnClickListener {
                val editText = EditText(this@MainActivity)
                editText.isFocusable = true

                // 打开输入对话框输入图片名称
                AlertDialog.Builder(this@MainActivity)
                    .setTitle("请输入图片名称")
                    .setView(editText)
                    .setNegativeButton("取消", null)
                    .setPositiveButton("确定") { _, _ ->
                        // 打开系统相册
                        tempName = editText.text.toString()
                        val intent = Intent(Intent.ACTION_OPEN_DOCUMENT).apply {
                            addCategory(Intent.CATEGORY_OPENABLE)
                            type = "image/*"
                        }
                        resultHandler.launch(intent)
                    }.show()
            }

            // 搜索功能
            searchView.setOnQueryTextListener(object : SearchView.OnQueryTextListener {
                override fun onQueryTextSubmit(query: String?): Boolean {
                    if (!(query == null || query == "")) {
                        initList(query)
                        imageAdapter.flushDataSet()
                    }
                    return false
                }

                override fun onQueryTextChange(newText: String?): Boolean {
                    if (newText == null || newText == "") {
                        initList(null)
                        imageAdapter.flushDataSet()
                    }
                    return false
                }

            })
        }
    }

    // 从数据库中获取数据
    private fun initList(query: String?) {
        val listData = if (query == null) imageDao.loadAllItems()
                        else imageDao.queryItems(query)
        imageList.clear()
        listData.observe(this) { list ->
            list.forEach { entity ->
                BufferedInputStream(FileInputStream(File(fileDir, entity.fileName))).use {
                    val bitmap = BitmapFactory.decodeStream(it)
                    imageList.add(ImageContent(entity.id, entity.name, entity.createTime, entity.fileName, bitmap))
                }
            }
            listData.removeObservers(this)
        }
    }


    inner class ImageAdapter : RecyclerView.Adapter<ImageAdapter.ViewHolder>() {
        inner class ViewHolder(val view: View) : RecyclerView.ViewHolder(view) {
            val imageView: ImageView = view.findViewById(R.id.sketch_image)
            val nameText: TextView = view.findViewById(R.id.name_text)
            val timeText: TextView = view.findViewById(R.id.time_text)
            val deleteBtn: ImageView = view.findViewById(R.id.delete_btn)
        }

        fun addItem(imageContent: ImageContent) {
            val query = binding.searchView.query
            if (query == null || query == "" || imageContent.name.contains(query)) {
                imageList.add(imageContent)
                notifyItemInserted(itemCount - 1)
                notifyItemChanged(itemCount - 1)
            }
        }

        fun flushDataSet() {
            notifyDataSetChanged()
        }

        override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): ViewHolder {
            val view = LayoutInflater.from(parent.context)
                .inflate(R.layout.image_item, parent, false)
            val viewHolder = ViewHolder(view)

            // 点击列表项将跳转到另一个页面
            viewHolder.view.setOnClickListener {
                val imageContent = imageList[viewHolder.adapterPosition]
                val intent = Intent(this@MainActivity, ShowActivity::class.java).apply {
                    putExtra("fileName", imageContent.fileName)
                }
                startActivity(intent)
            }

            // 删除列表项
            viewHolder.deleteBtn.setOnClickListener {
                val imageContent = imageList[viewHolder.adapterPosition]
                val originFile = File(fileDir, imageContent.fileName)
                val sketchFile = File(fileDir, "origin-${imageContent.fileName}")
                if (!originFile.delete() || !sketchFile.delete()) {
                    throw RuntimeException()
                }
                thread {
                    imageDao.deleteItem(imageContent.id)
                    imageList.remove(imageContent)
                    runOnUiThread {
                        notifyItemRemoved(viewHolder.adapterPosition)
                        notifyItemChanged(viewHolder.adapterPosition)
                    }
                }
            }
            return viewHolder
        }

        override fun onBindViewHolder(holder: ViewHolder, position: Int) {
            val image = imageList[position]
            holder.nameText.text = image.name
            holder.timeText.text = image.createTime
            holder.imageView.setImageBitmap(image.bitmap)
        }

        override fun getItemCount() = imageList.size
    }
}