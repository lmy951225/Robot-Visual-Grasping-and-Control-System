﻿#ifndef AMC_DYNAMIC_SPLINE_H_
#define AMC_DYNAMIC_SPLINE_H_

#include "../core/basic_type.hpp"

namespace amc::dynamic
{
	auto s_akima(Size n, const double *x, const double *y, double *p1, double *p2, double *p3, double zero_check = 1e-10)->void;
	auto s_akima_at(Size n, const double *x, const double *y, const double *p1, const double *p2, const double *p3, double x_1, const char order = '0')->double;
}

#endif
