#pragma once

#include <vector>

#include "imageprocess.h"
using namespace std;

namespace objr {
	using namespace std;
	int objr(vector<im::Object> &v, const BYTE *, const LONG Width, const LONG Height);
}