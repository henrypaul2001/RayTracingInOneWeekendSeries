#include "interval.h"
#include "rtweekend.h"

const interval interval::empty = interval(+infinity, -infinity);
const interval interval::universe = interval(-infinity, +infinity);

interval::interval() : min(+infinity), max(-infinity) {}