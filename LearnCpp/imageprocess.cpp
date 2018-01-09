#include "imageprocess.h"
#include "connect.h"
#include "bmp.h"

#include <vector>
using namespace std;

namespace im {
	BYTE OTSUThresholdSegmentation(BYTE *buff, LONG N);
	int SingleCCRectangle(const BYTE *buff,int * lable_buff, LONG Width, LONG Height, int lable, Rectangle & rec);
	int GenCC(const BYTE *, int *, LONG Width, LONG Height, int max_lable, vector<Object> &);
	int im(BYTE * imgbuff, LONG Width, LONG Height, vector<Object> &v)
	{
		if (!imgbuff)
			return -1;

		LONG N = Width * Height;
		BYTE * buff = new BYTE[N * 3];
		for (LONG i = 0; i < N; i++) {
			buff[i] = (imgbuff[3 * i] * 144 + 587 * imgbuff[3 * i + 1] + 299 * imgbuff[3 * i + 2] + 500) / 1000;
		}

		/* threshold segmentation with otsu algorithm. */
		OTSUThresholdSegmentation(buff, N);
		/* find the connected components. */
		int * lable_buff = new int[N];
		ConnectedComponents cc(20);
		const auto max_lable = cc.connected(buff, lable_buff, Width, Height, true);

		GenCC(buff, lable_buff, Width, Height, max_lable, v);

		delete[] lable_buff;
		delete[] buff;
		return 0;
	}
	
	int GenCC(const BYTE * buff, int * lable_buff, LONG Width, LONG Height, int max_lable, vector<Object> &v)
	{
		Rectangle rec = Rectangle();

	for (int i = 0; i < max_lable; i++)
	{
		SingleCCRectangle(buff, lable_buff, Width, Height, i, rec);
		if (rec.is_valid(Width, Height)) {
			Object o = Object(lable_buff, rec, i, Width, Height);
			v.push_back(o);
		}
	}
	
	for (size_t j = 0; j < v.size(); j++)
		v[j].make_bit_image();
	return 0;
	}
}

/* Accept the grayscale images only. */
BYTE im::OTSUThresholdSegmentation(BYTE * buff, LONG N)
{
	if (!buff)
		return 0;

	double Histogram[256] = { 0 };
	for (LONG n = 0; n < N; n++)
		// compute the image histogram
		Histogram[buff[n]]++;

	double sum = 0;
	for (size_t m = 0; m < 256; m++)
		sum += m * Histogram[m];
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

int im::SingleCCRectangle(const BYTE *buff, int * lable_buff, LONG Width, LONG Height, int lable, Rectangle & rec)
{
	bool first_pixel=0;
	rec = Rectangle();
	for (LONG r = 0; r < Height; r++) {
		int * row = &lable_buff[Width*r];
		for (LONG c = 0; c < Width; c++) {
			if (!buff[Width*r + c] && row[c] == lable) {
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

