package com.scut.example

import androidx.room.Database
import androidx.room.Room
import androidx.room.RoomDatabase

@Database(version = 1, entities = [ImageEntity::class], exportSchema = false)
abstract class ImageDatabase: RoomDatabase() {
    abstract fun imageDao(): ImageDao

    companion object {
        @Volatile
        private var instance: ImageDatabase? = null

        fun getDatabase(): ImageDatabase {
            instance?.let { return it }
            synchronized(ImageDatabase::class) {
                return Room.databaseBuilder(ImageApplication.context,
                        ImageDatabase::class.java, "image_database")
                        .build().apply { instance = this }
            }
        }
    }
}