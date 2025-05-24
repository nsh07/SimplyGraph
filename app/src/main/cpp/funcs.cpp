//
// Created by nishant on 24/05/25.
//

#include <math.h>

#include "include/funcs.h"

bool approxEqual(double a, double b, double eps) {
    return (a != 0.0 ? abs((a - b) / a) < eps : a == b);
}