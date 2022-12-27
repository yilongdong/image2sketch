package com.scut.example

import androidx.room.Entity
import androidx.room.PrimaryKey
import java.sql.Timestamp
import java.util.UUID

@Entity(tableName = "tb_image")
data class ImageEntity(
    var fileName: String,
    var name: String,
    var createTime: String
) {
    @PrimaryKey(autoGenerate = true)
    var id: Long = 0
}
