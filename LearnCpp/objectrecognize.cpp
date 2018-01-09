#include <sstream>
#include <iostream>
#include <iomanip>
#include <math.h>
#include <stdlib.h>
#include "objectrecognize.h"
#include "bmp.h"

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
	void getfeatures(Features &f, im::Object& o, const LONG Width, const LONG Height);
}

int objr::objr(vector<im::Object>& v, const BYTE * origin, const LONG Width, const LONG Height)
{
	for (size_t i = 0; i < v.size(); i++) {
		Features f;
		getfeatures(f, v[i],Width, Height);

		cout.flags(ios::left);
		cout << v[i].lable << "\t";
		cout << setw(10) << f.DutyCycle << "\t";
		cout << setw(10) << f.Hu1 << "\t";
		cout << setw(10) << f.Hu2 << "\t";
		cout << setw(10) << f.AspectRatio << "\t";
		cout << setw(10) << f.Hollow << "\t";
		cout << endl;

		/* ÂÝÄ¸ */
		if (!v[i].is_valid && !f.Hollow && 0.48 < f.DutyCycle && f.DutyCycle < 0.63) {
			v[i].kind = Nut;
			v[i].is_valid = true;
		}
		/* Ó²±Ò */
		if (!v[i].is_valid && 0.75 < f.DutyCycle && f.DutyCycle < 0.84 && f.AspectRatio > 0.96 ) {
			v[i].kind = Coin;
			v[i].is_valid = true;
		}
		/* Ë¿¸Ë */
		if (!v[i].is_valid && 0.25 > f.DutyCycle && abs(f.Hu1 - 1.6) < 0.2) {
			v[i].kind = HexKey;
			v[i].is_valid = true;
		}
		/* ÂÝË¿ */
		if (!v[i].is_valid  &&
			(abs(f.Hu1 - 0.35) < 0.06 || (abs(f.Hu1 - 0.47) < 0.06))) {
			v[i].kind = Screw;
			v[i].is_valid = true;
		}


		//if (v[i].is_valid) {
		if(true){
			char filename[50];
			sprintf_s(filename, "./test/%d_%s.bmp", v[i].lable, ObjectKindName[v[i].kind]);
			MonochromeBmpHandler imghandle = MonochromeBmpHandler();
			imghandle.write(v[i].obuff.pixel(0), (char*)filename, v[i].obuff.Width, v[i].obuff.Height);
		}
	}
	return 0;
}

void objr::getfeatures(Features &f, im::Object& o, const LONG Width, const LONG Height)
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
	f.Hu1 = abs(u20 + u02);
	f.Hu2 = abs((u20 - u02)*(u20 - u02) + 4 * u11 * u11);
}