#include <jni.h>
#include <string>
#include <android/bitmap.h>
#include <png.h>
#include <zlib.h>

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_ihuntto_libpng_PNG_00024Companion_save(JNIEnv *env, jobject thiz, jobject bitmap,
                                                jstring file_path) {
    const char *path = env->GetStringUTFChars(file_path, nullptr);
    if (path == nullptr) {
        return JNI_FALSE;
    }

    // 获取 Bitmap 信息
    AndroidBitmapInfo info;
    if (AndroidBitmap_getInfo(env, bitmap, &info) < 0) {
        env->ReleaseStringUTFChars(file_path, path);
        return JNI_FALSE;
    }

    if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
        // 需要 RGBA_8888 格式
        env->ReleaseStringUTFChars(file_path, path);
        return JNI_FALSE;
    }

    // 锁定 Bitmap 像素
    void *pixels;
    if (AndroidBitmap_lockPixels(env, bitmap, &pixels) < 0) {
        env->ReleaseStringUTFChars(file_path, path);
        return JNI_FALSE;
    }

    FILE *fp = fopen(path, "wb");
    if (!fp) {
        AndroidBitmap_unlockPixels(env, bitmap);
        env->ReleaseStringUTFChars(file_path, path);
        return JNI_FALSE;
    }

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png) {
        fclose(fp);
        AndroidBitmap_unlockPixels(env, bitmap);
        env->ReleaseStringUTFChars(file_path, path);
        return JNI_FALSE;
    }

    png_infop info_ptr = png_create_info_struct(png);
    if (!info_ptr) {
        png_destroy_write_struct(&png, nullptr);
        fclose(fp);
        AndroidBitmap_unlockPixels(env, bitmap);
        env->ReleaseStringUTFChars(file_path, path);
        return JNI_FALSE;
    }

    if (setjmp(png_jmpbuf(png))) {
        png_destroy_write_struct(&png, &info_ptr);
        fclose(fp);
        AndroidBitmap_unlockPixels(env, bitmap);
        env->ReleaseStringUTFChars(file_path, path);
        return JNI_FALSE;
    }

    png_init_io(png, fp);

    // 1. 设置最快的压缩级别
    png_set_compression_level(png, Z_BEST_SPEED);

    // 2. 禁用所有过滤器（最快）
    png_set_filter(png, PNG_FILTER_TYPE_BASE, PNG_FILTER_NONE);

    // 3. 设置压缩策略为最快
    png_set_compression_strategy(png, Z_DEFAULT_STRATEGY);

    // 设置 PNG 头信息
    int color_type = PNG_COLOR_TYPE_RGBA;
    png_set_IHDR(png, info_ptr, info.width, info.height, 8, color_type,
                 PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE,
                 PNG_FILTER_TYPE_BASE);

    png_write_info(png, info_ptr);

    // 写入图像数据
    png_bytep *row_pointers = new png_bytep[info.height];
    for (int y = 0; y < info.height; y++) {
        row_pointers[y] = static_cast<png_bytep>(pixels) + y * info.stride;
    }

    png_write_image(png, row_pointers);
    png_write_end(png, nullptr);

    // 清理资源
    delete[] row_pointers;
    png_destroy_write_struct(&png, &info_ptr);
    fclose(fp);
    AndroidBitmap_unlockPixels(env, bitmap);
    env->ReleaseStringUTFChars(file_path, path);

    return JNI_TRUE;
}