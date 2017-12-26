#include <sstream>

#include "objectrecognize.h"
#include "bmp.h"

using namespace std;

int objr::objr(vector<im::Object>& v)
{
	for (int i = 0; i < v.size(); i++) {
		v[i].kind = Screw;
		stringstream filename;
		filename << "object_lable_" << v[i].lable << ".bmp";
		MonochromeBmpHandler imghandle = MonochromeBmpHandler();
		imghandle.write(v[i].obuff.pixel(0), filename.str(), v[i].obuff.Width, v[i].obuff.Height);
	}
	return 0;
}
