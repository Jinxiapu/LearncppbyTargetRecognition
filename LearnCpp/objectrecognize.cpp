#include <sstream>

#include "objectrecognize.h"
#include "bmp.h"

using namespace std;

int objr::objr(vector<im::Object>& v)
{
	for (int i = 0; i < v.size(); i++) {
		v[i].kind = Screw;
		char filename[30];
		sprintf(filename, "./test/object_lable_%d.bmp", v[i].lable);
		MonochromeBmpHandler imghandle = MonochromeBmpHandler();
		imghandle.write(v[i].obuff.pixel(0), (char*)filename, v[i].obuff.Width, v[i].obuff.Height);
	}
	return 0;
}
