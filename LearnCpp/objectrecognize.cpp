#include <sstream>
#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include "objectrecognize.h"
#include "bmp.h"
#include "imageprocess.h"

#include <vector>

using namespace std;

char* ObjectKindName[6] = { "Useless", "UnKnownKind", "Screw", "Nut", "HexKey", "Coin" };

namespace objr {
	struct Features
	{
		double DutyCycle;
		double Hu1;
		double Hu2;
		double AspectRatio;
		bool Hollow;
	};
	void getfeatures(Features &f, im::Object& o);
	int Erosion(im::ObjectBuff &ob, int size);
	int SplitCC(im::ObjectBuff &ob, vector<im::Object> &v);
	bool ObjectRec(im::Object &o);
	class CornerPoint
	{
	public:
		CornerPoint(LONG w, LONG h, BYTE* ebuff, int size) :
			w(w), h(h), ebuff(ebuff), size(size) {};
		int  operator()(const BYTE* obuff) {
			for (LONG y = 0; y < h; y++) {
				for (LONG x = 0; x < w; x++) {
					if (!*getp(x, y))
						continue;
					int M[2][2] = { 0 };
					int p = 2;
					for (int i = p; i < size+p; i++) {
						for (int j = p; j < size+p; j++) {
							M[0][0] += getp(x - j, y - i) ? *getp(x - j, y - i) / 255 : 0;
							M[0][1] += getp(x - j, y + i) ? *getp(x - j, y + i) / 255 : 0;
							M[1][0] += getp(x + j, y - i) ? *getp(x + j, y - i) / 255 : 0;
							M[1][1] += getp(x + j, y + i) ? *getp(x + j, y + i) / 255 : 0;
						}
					}
					int flag = 0;
					for (int m = 0; m < 4; m++) {
						if (((int*)M)[m] > 1)
							flag++;
					}
					if (flag >= 3 && *getp(x,y)) {
						LONG min_d = 1000;
						int flag_n;
						for (size_t n = 0; n < v.size(); n++) {
							if (abs(x - v[n].x) + abs(y - v[n].y) < min_d) {
								min_d = abs(x - v[n].x) + abs(y - v[n].y);
								flag_n = n;
							}
						}
						if (v.empty() || min_d > 6) {
							v.push_back(Point(x, y));
						}
						else {
							v[flag_n].x = (v[flag_n].x + x)/2;
							v[flag_n].y = (v[flag_n].y + y) / 2;
						}
					}	
				}
			}

			std::cout << "-------------------ReRec--------------------" << endl;
			std::cout << "corner point:" << v.size() << endl;
			for (size_t n = 0; n < v.size(); n++) {
				*getp(v[n].x, v[n].y) = 128;
				std::cout << v[n].x<< "," <<  v[n].y << "\t"<< endl;
			}
			std::cout << "-------------------ReRec--------------------" << endl;
			return 0;
		}

		int draw0line(BYTE* buff) {
			for (size_t i = 0; i < v.size(); i++) {
				for (size_t j = i; j < v.size(); j++) {
					if (abs(v[i].x - v[j].x) > (w / 5) || abs(v[i].y - v[j].y) > (h / 5))
						continue;
					else
						bresenham(buff, v[i].x, v[i].y, v[j].x, v[j].y);
				}
			}
			return 0;
		}

		LONG w;
		LONG h;
		BYTE* ebuff;
		int size;
		struct Point {
			Point(LONG x, LONG y) :x(x), y(y) {}
			LONG x;
			LONG y;
		};
		vector<Point> v;
	private:
		BYTE * getp(LONG x, LONG y) {
			if (x < 0 || y < 0)
				return NULL;
			if (x >= w || y >= h)
				return NULL;
			return &ebuff[y*w + x];
		}
		void bresenham(BYTE* buff, int x0, int y0, int x1, int y1) {
			int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
			int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
			int err = (dx > dy ? dx : -dy) / 2;

			while (buff[w*y0 + x0] = 0, x0 != x1 || y0 != y1) {
				int e2 = err;
				if (e2 > -dx) { err -= dy; x0 += sx; }
				if (e2 <  dy) { err += dx; y0 += sy; }
			}
		}
	};
}

double dabs(double x){
	return x > 0.0 ? x:-x;
}

bool objr::ObjectRec(im::Object &o)
{
	Features f;
	getfeatures(f, o);

	/* 螺母 */
	if (!o.is_valid && !f.Hollow && 0.48 < f.DutyCycle &&
		f.DutyCycle < 0.63 && (f.Hu1>0.2 && f.Hu1 < 0.4) &&
		f.Hu2 < 0.001) {
		o.kind = Nut;
		o.is_valid = true;
	}
	/* 硬币 */
	if (!o.is_valid && 0.75 < f.DutyCycle && f.DutyCycle < 0.84 && f.AspectRatio > 0.96) {
		o.kind = Coin;
		o.is_valid = true;
	}
	/* 丝杆 */
	if (!o.is_valid && 0.25 > f.DutyCycle && dabs(f.Hu1 - 1.6) < 0.2) {
		o.kind = HexKey;
		o.is_valid = true;
	}
	/* 螺丝 */
	if (!o.is_valid &&
		(((dabs(f.Hu1 - 0.33) < 0.04) && (dabs(f.Hu2 - 0.075) < 0.03)) ||
		((dabs(f.Hu1 - 0.47) < 0.05) && (dabs(f.Hu2 - 0.17) < 0.05)))) {
		o.kind = Screw;
		o.is_valid = true;
	}

	std::cout.flags(ios::left);
	std::cout << o.lable << "\t";
	std::cout << setw(10) << f.DutyCycle << "\t";
	std::cout << setw(10) << f.Hu1 << "\t";
	std::cout << setw(10) << f.Hu2 << "\t";
	std::cout << setw(10) << f.AspectRatio << "\t";
	std::cout << setw(10) << f.Hollow << "\t";
	std::cout << setw(10) << ObjectKindName[o.kind] << "\t";
	std::cout << endl;

	return o.is_valid;
}


int objr::objr(vector<im::Object>& v, const BYTE * origin, const LONG Width, const LONG Height, bool split)
{
	vector<im::Object> v_split;
	for (size_t i = 0; i < v.size(); i++) {
		/* 第一遍识别时如果连通域过大说明是边框，直接进入去粘连阶段 */
		if (v[i].obuff.Width < 0.9 * Width && v[i].obuff.Width < 0.9 * Height) {
			ObjectRec(v[i]);
		} else {
			v[i].kind = Useless;
		}

		if (split && !v[i].is_valid) {
			size_t v_split_size = v_split.size();
			bool recflag = false; // 是否有识别到新东西
			if (SplitCC(v[i].obuff, v_split) == 0) {
				for (size_t m = v_split_size; m < v_split.size(); m++) {
					if (v_split[m].obuff.Width < 0.9 * Width && v_split[m].obuff.Width < 0.9 * Height) {
						if (ObjectRec(v_split[m]))
							recflag = true;
					}
					else {
						v_split[m].kind = Useless;
						recflag = true;
					}
					v_split[m].rec.min_x += v[i].rec.min_x;
					v_split[m].rec.max_x += v[i].rec.min_x;
					v_split[m].rec.min_y += v[i].rec.min_y;
					v_split[m].rec.max_y += v[i].rec.min_y;
				}
			}
			if (recflag)
				v[i].kind = Useless;
		}

	}
	if (split) {
		for (size_t y = 0; y < v_split.size(); y++) {
			if (v_split[y].is_valid)
				v.push_back(v_split[y]);
		}
	}
	return 0;
}

void objr::getfeatures(Features &f, im::Object& o)
{
	double M[3][3] = { 0.0 };
	for (int r = 0; r < o.obuff.Width; r++)
	{
		for (int c = 0; c < o.obuff.Height; c++)
		{
			if (*o.obuff.pixel(r, c)) {
				M[0][0] += 1;
				M[1][0] += r;
				M[0][1] += c;
				M[1][1] += r * c;
				M[0][2] += c * c;
				M[2][0] += r * r;
			}
		}
	}
	double w = o.obuff.Width;
	double h = o.obuff.Height;

	f.AspectRatio = (w < h) ? (w / h) : (h / w);
	f.DutyCycle = M[0][0] / o.obuff.Height / o.obuff.Width;
	f.Hollow = *o.obuff.pixel(o.obuff.Width / 2, o.obuff.Height / 2);

	double ax = M[1][0] / M[0][0];
	double ay = M[0][1] / M[0][0];
	double u11 = M[1][1] - ay * M[1][0];
	double u20 = M[2][0] - ax * M[1][0];
	double u02 = M[0][2] - ay * M[0][1];
	u11 = u11 / M[0][0] / M[0][0];
	u20 = u20 / M[0][0] / M[0][0];
	u02 = u02 / M[0][0] / M[0][0];
	f.Hu1 = u20 + u02;
	f.Hu1 = f.Hu1>0?f.Hu1: 0-f.Hu1;
	f.Hu2 = (u20 - u02)*(u20 - u02) + 4 * u11 * u11;
	f.Hu2 = f.Hu2>0?f.Hu2: 0-f.Hu2;
}

int objr::SplitCC(im::ObjectBuff &ob, vector<im::Object> &v) {
	BYTE * temp = new BYTE[ob.Height*ob.Width];
	int v_size = v.size();
	int Y[3][3] = { {-1, -2, -1}, 
					{0, 0, 0},
					{1, 2 ,1} };
	int X[3][3] = { { -1, 0, 1 },
					{ -2, 0, 2 } ,
					{ -1, 0 ,1 } };
	/* 用sobel 算子做边缘提取 */
	for (int r = 0; r < ob.Height; r++) {
		for (int c = 0; c < ob.Width; c++) {
			int M[3][3] = { 0 };
			int dx = 0, dy = 0;
			
			for (int i = -1; i < 2; i++) {
				for (int j = -1; j < 2; j++) {
					if (!ob.pixel(c + i, r + j)) {
						M[1 + i][1 + j] = 0;
						continue;
					}
					M[1 + i][1 + j] = *ob.pixel(c + i, r + j);
				}
			}

			if (M[1][1] == 255)
				M[1][1] = 254;

			for (int n = 0; n < 9; n++) {
				dx += ((int*)X)[n] * ((int*)M)[n];
				dy += ((int*)Y)[n] * ((int*)M)[n];
			}
			BYTE it = abs(dx) + abs(dy) > 100 ? 255 : 0;
			temp[r*ob.Width + c] = it;
		}
	}

	/* 找凹点 */
	CornerPoint cp = CornerPoint(ob.Width, ob.Height, temp, 3);
	cp(ob.pixel(0));

	if (cp.v.size() < 2) 
		return 1; // 找不到两个以上凹点

	cp.draw0line(ob.pixel(0));
	int * lable_buff = new int[ob.Width*ob.Height];
	im::ConnectedComponents cc(20);
	const auto max_lable = cc.connected(ob.pixel(0), lable_buff, ob.Width, ob.Height, false);
	im::GenCC(ob.pixel(0), lable_buff, ob.Width, ob.Height, max_lable, v);
	if (v.size() - v_size <= 1)
		return 2; // 分割后还是只有一个连通域。


	delete[] lable_buff;
	delete[] temp;
	return 0;
}



