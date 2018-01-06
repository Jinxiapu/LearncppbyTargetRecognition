#include <sstream>
#include <iostream>
#include <iomanip>

#include "objectrecognize.h"
#include "bmp.h"

using namespace std;

char* ObjectKindName[6] = { "Useless", "UnKnownKind", "Screw", "Nut", "HexKey", "Coin" };

namespace objr {
	void getfeatures(double *features, im::Object& o, const LONG Width, const LONG Height);
}

int objr::objr(vector<im::Object>& v, const BYTE * origin, const LONG Width, const LONG Height)
{
	double NutArea = 0;
	for (size_t i = 0; i < v.size(); i++) {
		v[i].kind = UnknownKind;
		v[i].is_valid = true;

		double features[4];
		bool RecOver = false;
		getfeatures(features, v[i], Width, Height);
		if (features[3] == 0 && 0.48<features[2] && 0.63>features[2]) {
			v[i].kind = Nut;
			RecOver = true;
			NutArea = features[1];
			v[i].is_valid = true;
		}
			
		if (!RecOver &&  features[1] >NutArea &&
			0.75 < features[2] && 
			(features[0] - 1) < 0.1 && (features[0] - 1) > -0.1) {
			v[i].kind = Coin;
			RecOver = true;
		}

		if (!RecOver &&  features[2] >0.24) {
			v[i].kind = Screw;
			RecOver = true;
		}

		if(!RecOver)
			v[i].kind = HexKey;

		cout.flags(ios::left);
		cout << v[i].lable << "\t";
		for (size_t i = 0; i < 4; i++)
		{
			cout << setw(10) << features[i] << "\t";
		}

		cout << endl;

		//if (v[i].is_valid) {
		if (true){
			char filename[50];
			sprintf_s(filename, "./test/object_lable_%d_kind_%s.bmp", v[i].lable, ObjectKindName[v[i].kind]);
			MonochromeBmpHandler imghandle = MonochromeBmpHandler();
			imghandle.write(v[i].obuff.pixel(0), (char*)filename, v[i].obuff.Width, v[i].obuff.Height);
		}
	}
	return 0;
}

void objr::getfeatures(double *features, im::Object& o, const LONG Width, const LONG Height)
{
	double oarea = o.obuff.Height * o.obuff.Width;
	if (o.obuff.Width < o.obuff.Height)
		features[0] = (double)o.obuff.Width / (double)o.obuff.Height;
	else
		features[0] = (double)o.obuff.Height / (double)o.obuff.Width;
	
	
	double objectpixels = 0;
	for (int r =0; r < o.obuff.Height; r++)
	{
		for(int c = 0; c < o.obuff.Width; c++)
		{
			if (*o.obuff.pixel(c, r)) {
				objectpixels += 1;
			}
		}
	}

	features[1] = objectpixels / (Width*Height);
	features[2] = objectpixels / oarea; // Object占空比

	features[3] = 0; // Object中心矩阵非0像素
	for (int rp = -1; rp < 2; rp++) {
		for (int cp = -1; cp < 2; cp++) {
			features[3] += (*o.obuff.pixel(o.obuff.Width / 2 + cp, o.obuff.Height / 2 + rp)) ? 1 : 0;
		}
	}
}