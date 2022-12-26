package com.scut.example

import androidx.lifecycle.LiveData
import androidx.room.Dao
import androidx.room.Delete
import androidx.room.Insert
import androidx.room.Query

@Dao
interface ImageDao {
    @Insert
    fun insertItem(imageEntity: ImageEntity): Long

    @Query("select * from tb_image")
    fun loadAllItems(): LiveData<List<ImageEntity>>

    @Query("delete from tb_image where id = :id")
    fun deleteItem(id: Long)
}