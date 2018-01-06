#include "imageprocess.h"
#include "bmp.h"
#include <vector>
using namespace std;

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
	imghandle.write(buff, "./test/after.bmp", width, height, SingleOrBGR);
	getchar();
	return 0;
}

