#ifndef _OBJR_H
#define _OBJR_H


#include <vector>

#include "imageprocess.h"
using namespace std;

namespace objr {
	using namespace std;
	int objr(vector<im::Object> &v, const BYTE *, const LONG Width, const LONG Height, bool split);
}

#endif // !_BMP_H