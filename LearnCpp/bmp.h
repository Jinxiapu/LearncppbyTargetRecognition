#pragma once
#include <fstream>
#include <Windows.h>
#include <iostream>
#pragma pack(2)

class MonochromeBmpHandler
{
public:
	MonochromeBmpHandler() {};

	BYTE* read(const char* filename, LONG & width, LONG & height) {
		if (!filename)
			return NULL;
		
		std::ifstream fin(filename, std::ios::binary);
		if (!fin)
			return NULL;

		fin.seekg(0, std::ios::beg);
		fin.read((char*)&bmfh, sizeof(bmfh));
		fin.read((char*)&bmih, sizeof(bmih));

		width = this->bmih.biWidth;
		height = this->bmih.biHeight;

		fin.seekg(bmfh.bfOffBits, std::ios::beg);
		size_t length =  width*height;
		BYTE * buff = new BYTE[length];
		size_t  skip = 4 - ((this->bmih.biWidth * 8) >> 3) & 3;
		for (LONG r = 0; r < height; r++) {
			BYTE *row = &buff[r*width];
			fin.read((char *)row, width);
			fin.seekg(skip, std::ios::cur);
		}
		return buff;
	}

	bool write(const BYTE * buff, const char* filename, LONG width, LONG height) {
		if (!filename || !buff)
			return false;

		std::ofstream fout(filename, std::ios::binary | std::ios::trunc);
		if (!fout) 
			return false;

		RGBQUAD palette[256];
		for (int i = 0; i < 256; i++) {
			BYTE bytei = (BYTE)i;
			palette[i].rgbBlue = bytei;
			palette[i].rgbGreen = bytei;
			palette[i].rgbRed = bytei;
			palette[i].rgbReserved = 0;
		}

		bmfh.bfType = 0x4d42;
		bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD);
		bmfh.bfSize = bmfh.bfOffBits+width*height;
		bmfh.bfReserved1 = 0;
		bmfh.bfReserved2 = 0;

		bmih.biSize = (sizeof(BITMAPINFOHEADER));
		bmih.biWidth = width;
		bmih.biHeight = height;
		bmih.biPlanes = 1;
		bmih.biBitCount = 8;
		bmih.biCompression = BI_RGB; //no compression needed
		bmih.biSizeImage = 0;
		bmih.biXPelsPerMeter = 0;
		bmih.biYPelsPerMeter = 0;
		bmih.biClrUsed = 0;
		bmih.biClrImportant = 0;

		fout.write((char*)&bmfh, sizeof(bmfh));
		fout.write((char*)&bmih, sizeof(bmih));
		fout.write((char*)&palette, 256 * sizeof(RGBQUAD));

		size_t  skip = 4 - ((this->bmih.biWidth * 8) >> 3) & 3;
		const char * zeros[4] = { 0, 0, 0, 0 };

		for (LONG r = 0; r < height; r++) {
			const BYTE *row = &buff[r*width];
			fout.write(((const char *)row), width);
			fout.write((char*)zeros, skip);
		}

		return true;
	}
private:
	BITMAPFILEHEADER bmfh;
	BITMAPINFOHEADER bmih;
};

class BmpHandler
{
public:
	BmpHandler() {};

	BYTE* read(const char* filename, LONG & width, LONG & height, bool & SingleOrBGR) {
		if (!filename)
			return NULL;

		std::ifstream fin(filename, std::ios::binary);
		if (!fin)
			return NULL;

		fin.seekg(0, std::ios::beg);
		fin.read((char*)&bmfh, sizeof(bmfh));
		fin.read((char*)&bmih, sizeof(bmih));

		if (bmih.biBitCount == 24)
			SingleOrBGR = false;
		else {
			if (bmih.biBitCount == 8)
				SingleOrBGR = true;
			else
				return NULL;
		}

		width = this->bmih.biWidth;
		height = this->bmih.biHeight;

		fin.seekg(bmfh.bfOffBits, std::ios::beg);
		BYTE * buff = new BYTE[width * height * (bmih.biBitCount / 8)];
		size_t  skip = 4 - ((this->bmih.biWidth * 8) >> 3) & 3;
		for (LONG r = 0; r < height; r++) {
			BYTE *row = &buff[r*width*(bmih.biBitCount / 8)];
			fin.read((char *)row, width*(bmih.biBitCount / 8));
			fin.seekg(skip*(bmih.biBitCount / 8), std::ios::cur);
		}
		return buff;
	}

	bool write(const BYTE * buff, const char* filename, LONG width, LONG height,const bool SingleOrBGR) {
		if (!filename || !buff)
			return false;

		std::ofstream fout(filename, std::ios::binary | std::ios::trunc);
		if (!fout)
			return false;

		bmfh.bfType = 0x4d42;
		bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) 
			+ (SingleOrBGR ? 256 : 0) * sizeof(RGBQUAD);
		bmfh.bfSize = bmfh.bfOffBits + width * height* (SingleOrBGR ? 1 : 3);
		bmfh.bfReserved1 = 0;
		bmfh.bfReserved2 = 0;

		bmih.biSize = (sizeof(BITMAPINFOHEADER));
		bmih.biWidth = width;
		bmih.biHeight = height;
		bmih.biPlanes = 1;
		bmih.biBitCount = (SingleOrBGR ? 8 : 24);
		bmih.biCompression = BI_RGB; //no compression needed
		bmih.biSizeImage = 0;
		bmih.biXPelsPerMeter = 0;
		bmih.biYPelsPerMeter = 0;
		bmih.biClrUsed = 0;
		bmih.biClrImportant = 0;

		fout.write((char*)&bmfh, sizeof(bmfh));
		fout.write((char*)&bmih, sizeof(bmih));
		if (SingleOrBGR) {
			RGBQUAD palette[256];
			for (int i = 0; i < 256; i++) {
				BYTE bytei = (BYTE)i;
				palette[i].rgbBlue = bytei;
				palette[i].rgbGreen = bytei;
				palette[i].rgbRed = bytei;
				palette[i].rgbReserved = 0;
			}
			fout.write((char*)&palette, 256 * sizeof(RGBQUAD));
		}
		

		size_t  skip = 4 - ((this->bmih.biWidth * 8) >> 3) & 3;
		const char * zeros[4*3] = { 0 };

		for (LONG r = 0; r < height; r++) {
			const BYTE *row = &buff[r*width*(SingleOrBGR ? 1 : 3)];
			fout.write(((const char *)row), width*(SingleOrBGR ? 1 : 3));
			fout.write((char*)zeros, skip*(SingleOrBGR ? 1 : 3));
		}

		return true;
	}
private:
	BITMAPFILEHEADER bmfh;
	BITMAPINFOHEADER bmih;
};