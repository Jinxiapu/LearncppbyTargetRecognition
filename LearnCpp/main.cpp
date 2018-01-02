#include "imageprocess.h"
#include "bmp.h"

int main()
{	
	LONG width = 0, height = 0;
	MonochromeBmpHandler imghandle = MonochromeBmpHandler();
	BYTE *buff = imghandle.read("./test/origin.bmp", width, height);

	im::im(buff, width, height);
	return !imghandle.write(buff, "./test/ConnectedComponents.bmp", width, height);
}

