﻿#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "Image.h"
#include "stb_image.h"
#include "stb_image_write.h"


Image::Image(const char* filename) {
	// 読み込みが成功したかどうか
	if (read(filename)) {
		printf("Read %s\n", filename);
		size = w * h * channels;
	}
	else {
		printf("Failed to read %s\n", filename);
	}
}

Image::Image(int w, int h, int channels) : w(w), h(h), channels(channels) {
	size = w * h * channels;
	data = new uint8_t[size];
}

Image::Image(const Image& img) : Image(img.w, img.h, img.channels) {
	memcpy(data, img.data, size);
}

Image::~Image() {
	stbi_image_free(data);
}

bool Image::read(const char* filename) {
	data = stbi_load(filename, &w, &h, &channels, 0);
	return data != NULL;
}

bool Image::write(const char* filename) {
	ImageType type = get_file_type(filename);
	int success;
	switch (type) {
	case PNG:
		success = stbi_write_png(filename, w, h, channels, data, w * channels);
		break;
	case BMP:
		success = stbi_write_bmp(filename, w, h, channels, data);
		break;
	case JPG:
		success = stbi_write_jpg(filename, w, h, channels, data, 100);
		break;
	case TGA:
		success = stbi_write_tga(filename, w, h, channels, data);
		break;
	}
	return success != 0;
}

ImageType Image::get_file_type(const char* filename) {
	const char* ext = strrchr(filename, '.');
	if (ext != nullptr) {
		if (strcmp(ext, ".png") == 0) {
			return PNG;
		}
		else if (strcmp(ext, ".jpg") == 0) {
			return JPG;
		}
		else if (strcmp(ext, ".bmp") == 0) {
			return BMP;
		}
		else if (strcmp(ext, ".tga") == 0) {
			return TGA;
		}
	}
	return PNG;
}

// グレースケール（単純な3チャンネルの平均）
Image& Image::grayscale_avg() {
	// (R+G+B)/3
	if (channels < 3) {
		printf("画像%pのチャネル数は3未満であり、すでにグレースケールだと思われます...", this);
	}
	else {
		for (int i = 0; i < size; i += channels) {
			//(r+g+b)/3
			int gray = (data[i] + data[i + 1] + data[i + 2]) / 3;
			memset(data + i, gray, 3);
		}
	}
	return *this;
}

// グレースケール（割合をかけるやり方）
Image& Image::grayscale_lum() {
	if (channels < 3) {
		printf("画像%pのチャネル数は3未満であり、すでにグレースケールだと思われます...", this);
	}
	else {
		for (int i = 0; i < size; i += channels) {
			// 0.2126*赤 + 0.7152 * 緑 + 0.0722 * 青
			int gray = 0.2126 * data[i] + 0.7152 * data[i + 1] + 0.0722 * data[i + 2];
			memset(data + i, gray, 3);
		}
	}
	return *this;
}