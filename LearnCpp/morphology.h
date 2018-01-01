#pragma once

#pragma once
#include <Windows.h>

namespace im {
	typedef bool Pixel;
	typedef LONG size_pixel;

	struct cKernel
	{
		cKernel(size_t size, const Pixel * kbuff) : size(size), buff(kbuff) {}

		Pixel get(int i, int j) {
			return buff[i*size + j];
		}
		Pixel get(int n) {
			return buff[n];
		}

		const size_t size;
		const Pixel *buff;
	};

	class Dilation
	{
	public:
		Dilation(bool* image, const size_pixel w, const size_pixel h, cKernel ck) :
			image(image), w(w), h(h), ck(ck) {}
		int run() {
			if (!(ck.size % 2))
				return -2;
			if (!image)
				return -1;
			bool * temp = new bool[w*h];
			for (int r = 0; r < h; r++)
				for (int c = 0; c < w; c++)
					temp[r*w + c] = d(&image[r*w + c], r, c);
			for (size_t i = 0; i < w*h; i++)
			{
				image[i] = temp[i];
			}
			delete[] temp;
			return 0;
		}
		bool * image;
		size_pixel w;
		size_pixel h;
		cKernel ck;
	private:
		bool d(const bool* self, const size_pixel r, const size_pixel c) {
			if (r < ck.size / 2 || c < ck.size / 2)
				return *self;
			if (r >= h - ck.size / 2 || c >= w - ck.size / 2)
				return *self;
			bool flag = false, end = false;
			int size = ck.size;
			for (int i = 0; (!end) && (i < size*size); i++)
				if (ck.get(i)) {
					bool it = *(self + ((i / size) - size / 2)*w + ((i % size) - size / 2));
					if (it & ck.get(i)) {
						flag = true;
						end = true;
					}
				}
			return flag;
		}
	};

	class Erosion
	{
	public:
		Erosion(bool* image, const size_pixel w, const size_pixel h, cKernel ck) :
			image(image), w(w), h(h), ck(ck) {}
		int run() {
			if (!(ck.size % 2))
				return -2;
			if (!image)
				return -1;
			bool * temp = new bool[w*h];
			for (int r = 0; r < h; r++)
				for (int c = 0; c < w; c++)
					temp[r*w + c] = e(&image[r*w + c], r, c);
			for (size_t i = 0; i < w*h; i++)
			{
				image[i] = temp[i];
			}
			delete[] temp;
			return 0;
		}
		bool * image;
		size_pixel w;
		size_pixel h;
		cKernel ck;
	private:
		bool e(const bool* self, const size_pixel r, const size_pixel c) {
			if (r < ck.size / 2 || c < ck.size / 2)
				return *self;
			if (r >= h - ck.size / 2 || c >= w - ck.size / 2)
				return *self;
			bool flag = true, end = false;
			int size = ck.size;
			for (int i = 0; !end && i < size*size; i++)
			{
				bool it = *(self + ((i / size) - size / 2)*w + ((i % size) - size / 2));
				if (ck.get(i)) {
					if (!(it & ck.get(i))) {
						flag = false;
						end = true;
					}
				}
			}
			return flag;
		}
	};
}

