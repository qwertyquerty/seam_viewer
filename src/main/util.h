#include "types.h"
#include "fsqrte.h"
#include <math.h>

#pragma once

const f32 FLT_EPSILON = 0.00000011920929;
const f32 G_CM3D_F_ABS_MIN = 32 * FLT_EPSILON;

inline bool cM3d_IsZero(f32 f) {
    return fabsf(f) < G_CM3D_F_ABS_MIN;
}

f32 cM3d_Len2dSq(f32 pX1, f32 pY1, f32 pX2, f32 pY2) {
    f32 xDiff = pX1 - pX2;
    f32 yDiff = pY1 - pY2;
    return xDiff * xDiff + yDiff * yDiff;
}

bool cM3d_Len2dSqPntAndSegLine(f32 param_1, f32 param_2, f32 param_3, f32 param_4, f32 p5, f32 p6, f32* param_7, f32* param_8, f32* param_9) {
    bool retVal = false;
    f32 f31 = p5 - param_3;
    f32 f30 = p6 - param_4;
    f32 len = f31 * f31 + f30 * f30;
    f32 f29;
    if (cM3d_IsZero(len)) {
        *param_9 = 0.0f;
        return false;
    } else {
        f29 = (f31 * (param_1 - param_3) + f30 * (param_2 - param_4)) / len;
        if (f29 >= 0.0f && f29 <= 1.0f) {
            retVal = true;
        }
        *param_7 = param_3 + f31 * f29;
        *param_8 = param_4 + f30 * f29;
        *param_9 = cM3d_Len2dSq(*param_7, *param_8, param_1, param_2);
        return retVal;
    }
}

struct Point {
    double x;
    double y;
};


double cross(const Point& A, const Point& B, const Point& P) {
    return (B.x - A.x) * (P.y - A.y) - (B.y - A.y) * (P.x - A.x);
}

int pointAboveLine(const Point& A, const Point& B, const Point& P) {
    double c = cross(A, B, P);

    if (c < 0) return 1;
    return 0;
}

f32 JMAFastSqrt(f32 x) {
    return fsqrte(x) * x;
}
