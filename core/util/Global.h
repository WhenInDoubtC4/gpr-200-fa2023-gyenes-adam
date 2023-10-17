#define _USE_MATH_DEFINES

#include <math.h>

constexpr int INFO_LOG_SIZE = 512;

namespace Util 
{
	inline constexpr float DegToRad(float deg)
	{
		return deg * (M_PI / 180.f);
	}
}