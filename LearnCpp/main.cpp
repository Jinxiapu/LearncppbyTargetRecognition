#include "imageprocess.h"
#include "bmp.h"
#include "objectrecognize.h"
#include <vector>
using namespace std;


inline void RGBcopy(BYTE * pixel, BYTE R, BYTE G, BYTE B)
{
	pixel[0] = B;
	pixel[1] = G;
	pixel[2] = R;
}

int main()
{	
	LONG width = 0, height = 0;
	BmpHandler imghandle = BmpHandler();
	bool SingleOrBGR;
	BYTE *buff = imghandle.read("./test/origin.bmp", width, height, SingleOrBGR);

	if (!buff) {
		std::cout << "error" << std::endl;
		getchar();
	}

	vector<im::Object> v;
	im::im(buff, width, height, v);

	objr::objr(v, buff, width, height, true);

	for (size_t n = 0; n < v.size(); n++) {
		for (LONG y = v[n].rec.min_y; y < v[n].rec.max_y; y++) {
			for (LONG x = v[n].rec.min_x; x < v[n].rec.max_x; x++) {
				if (*v[n].obuff.pixel(x - v[n].rec.min_x, y - v[n].rec.min_y)) {
					switch (v[n].kind)
					{
					case Nut:
						RGBcopy(&buff[(y*width + x) * 3], 0, 0, 255);
						break;
					case Screw:
						RGBcopy(&buff[(y*width + x) * 3], 0, 255, 255);
						break;
					case HexKey:
						RGBcopy(&buff[(y*width + x) * 3], 0, 255, 0);
						break;
					case Coin:
						RGBcopy(&buff[(y*width + x) * 3], 255, 0, 255);
						break;
					case UnknownKind:
						RGBcopy(&buff[(y*width + x) * 3], 255, 0, 0);
					default:
						break;
					}
				}
			}
		}
	}
	imghandle.write(buff, "./test/after.bmp", width, height, SingleOrBGR);
	
	int result[4] = { 0 };
	for (size_t nm = 0; nm < v.size(); nm++) {
		switch (v[nm].kind)
		{
		case Nut:
			result[0]++;
			break;
		case Screw:
			result[1]++;
			break;
		case HexKey:
			result[2]++;
			break;
		case Coin:
			result[3]++;
			break;
		default:
			break;
		}
	}
	cout << "��ĸ : " << result[0] << endl;
	cout << "��˿ : " << result[1] << endl;
	cout << "���� : " << result[2] << endl;
	cout << "Ӳ�� : " << result[3] << endl;

	getchar();
	return 0;
}

