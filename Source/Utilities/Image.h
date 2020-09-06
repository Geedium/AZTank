#pragma once
#include <string>
#include <FreeImage.h>

namespace Augiwne {
	static BYTE* LoadImage(const char* path, GLsizei* width, GLsizei* height) {
		FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
		fif = FreeImage_GetFileType(path, 0);
		if (fif == FIF_UNKNOWN) fif = FreeImage_GetFIFFromFilename(path);
		if (fif == FIF_UNKNOWN) return nullptr;
		FIBITMAP *dib = nullptr;
		if (FreeImage_FIFSupportsReading(fif)) dib = FreeImage_Load(fif, path);
		if (!dib) return nullptr;
		BYTE* pixels = FreeImage_GetBits(dib);
		*width = FreeImage_GetWidth(dib);
		*height = FreeImage_GetHeight(dib);
		int bits = FreeImage_GetBPP(dib);
		int size = *width * *height * (bits / 8);
		BYTE* result = new BYTE[size];
		memcpy(result, pixels, size);
		FreeImage_Unload(dib);
		return result;
	}
}