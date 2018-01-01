#include "imageprocess.h"
#include "connect.h"
#include "objectrecognize.h"
#include "bmp.h"
#include <string.h>

using namespace std;

namespace im {
	BYTE OTSUThresholdSegmentation(BYTE *buff, LONG N);
	int SingleCCRectangle(int * lable_buff, LONG Width, LONG Height, int lable, Rectangle & rec);
	int GenCC(int * lable_buff, LONG Width, LONG Height, int max_lable, vector<Object> &);
}

int im::im(BYTE * buff, LONG Width, LONG Height)
{
	if(!buff)
		return -1;

	LONG N = Width * Height;

	/* threshold segmentation with otsu algorithm. */
	OTSUThresholdSegmentation(buff, N);
	MonochromeBmpHandler imghandle = MonochromeBmpHandler();
	imghandle.write(buff, "./test/OTSUThresholdSegmentation.bmp", Width, Height);

	/* find the connected components. */
	int * lable_buff = new int[N];
	ConnectedComponents cc(20);
	int max_lable = cc.connected(buff, lable_buff, Width, Height, std::equal_to<BYTE>(), constant<bool, true>());
	std::cout << "totaly have " << max_lable << " lables." << std::endl;

	vector<Object> v;
	GenCC(lable_buff, Width, Height, max_lable, v);
	std::cout << "totaly have " << v.size()  << " valid lables." << std::endl;
	
	objr::objr(v);
	std::cout << "recognize result:" << std::endl;
	for (int i = 0; i < v.size(); i++) {
		std::cout << "lable: "<<v[i].lable;
		std::cout << " | " << v[i].rec << "\t";
		switch (v[i].kind)
		{
		case Useless:
			std::cout << "Useless";
			break;
		case Screw:
			std::cout << "Screw";
			break;
		case Nut:
			std::cout << "Nut";
			break;
		case HexKey:
			std::cout << "Hex key";
			break;
		case Coin:
			std::cout << "Coin";
			break;
		default:
			std::cout << "Unknown";
			break;
		}
		std::cout << std::endl;
	}

	for (LONG i = 0; i < N; i++)
		buff[i] = lable_buff[i] * (255 / max_lable);
	delete[] lable_buff;


	return 0;
}

/* Accept the grayscale images only. */
BYTE im::OTSUThresholdSegmentation(BYTE * buff, LONG N)
{
	if (!buff)
		return 0;

	double Histogram[256] = { 0 };
	for (size_t i = 0; i < N; i++)
		// compute the image histogram
		Histogram[buff[i]]++;

	double sum = 0;
	for (size_t i = 0; i < 256; i++)
		sum += i * Histogram[i];
	double q1 = 0, q2 = 0, sumB = 0, u1 = 0, u2 = 0;
	double var = 0, var_max = 0;
	BYTE best_t = 0;
	for (BYTE t = 0; t < 255; t++)
	{
		q1 += Histogram[t];
		if (q1 == 0)
			continue;
		q2 = N - q1;
		sumB += Histogram[t] * t; // sumB : the sum of pixels of backgroud.
		u1 = sumB / q1;
		u2 = (sum - sumB) / q2;

		var = q1 * q2*(u1 - u2)*(u1 - u2);
		if (var > var_max) {
			var_max = var;
			best_t = t;
		}			
	}

	for (LONG i = 0; i < N; i++)
		buff[i] = (buff[i] > best_t) ? 255 : 0;

	return best_t;
}

int im::SingleCCRectangle(int * lable_buff, LONG Width, LONG Height, int lable, Rectangle & rec)
{
	bool first_pixel=0;
	rec = Rectangle();
	for (LONG r = 0; r < Height; r++) {
		int * row = &lable_buff[Width*r];
		for (LONG c = 0; c < Width; c++) {
			if (row[c] == lable) {
				if (!first_pixel) {
					rec.min_x = c;
					rec.min_y = r;
				}
				first_pixel = 1;
				rec.max_x = (rec.max_x < c) ? c : rec.max_x;
				rec.max_y = (rec.max_y < r) ? r : rec.max_y;
				rec.min_x = (rec.min_x > c) ? c : rec.min_x;
				rec.min_y = (rec.min_y > r) ? r : rec.min_y;
			}
		}
	}
	return 0;
}

int im::GenCC(int * lable_buff, LONG Width, LONG Height, int max_lable, vector<Object> &v)
{
	Rectangle rec = Rectangle();

	for (int i = 0; i < max_lable; i++)
	{
		SingleCCRectangle(lable_buff, Width, Height, i, rec);
		if (rec.is_valid(Width, Height)) {
			Object o = Object(lable_buff, rec, i, Width, Height);
			v.push_back(o);
		}
	}
	for (Object &o : v)
		o.make_bit_image();
	return 0;
}