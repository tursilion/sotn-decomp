#include <common.h>
#include <psxsdk/libgte.h>
#include <log.h>
#include <math.h>

void InitGeom() { NOT_IMPLEMENTED; }

void SetGeomOffset(long ofx, long ofy) { NOT_IMPLEMENTED; }

void SetGeomScreen(long h) { NOT_IMPLEMENTED; }

int rsin(int a) {
    float x = (s16)a / 4096.f * M_PI * 2;
    return sin(x) * 4096.f;
}

int rcos(int a) {
    float x = (s16)a / 4096.f * M_PI * 2;
    return cos(x) * 4096.f;
}

long ratan2(long y, long x) {
    NOT_IMPLEMENTED;
    return 0;
}

long SquareRoot0(long a) {
    NOT_IMPLEMENTED;
    return 0;
}

long SquareRoot12(long a) {
    NOT_IMPLEMENTED;
    return 0;
}

MATRIX* RotMatrix(SVECTOR* r, MATRIX* m) {
    NOT_IMPLEMENTED;
    return 0;
}

MATRIX* TransMatrix(MATRIX* m, VECTOR* v) {
    NOT_IMPLEMENTED;
    return 0;
}

void SetRotMatrix(MATRIX* m) { NOT_IMPLEMENTED; }

void SetLightMatrix(MATRIX* m) { NOT_IMPLEMENTED; }

void SetColorMatrix(MATRIX* m) { NOT_IMPLEMENTED; }

void SetTransMatrix(MATRIX* m) { NOT_IMPLEMENTED; }

void SetBackColor(long rbk, long gbk, long bbk) { NOT_IMPLEMENTED; }

void func_80017008(VECTOR* v, long* l) { NOT_IMPLEMENTED; }

long RotAverageNclip3(SVECTOR* v0, SVECTOR* v1, SVECTOR* v2, long* sxy0,
                      long* sxy1, long* sxy2, long* p, long* otz, long* flag) {
    NOT_IMPLEMENTED;
    return 0;
}

long RotAverage3(SVECTOR* v0, SVECTOR* v1, SVECTOR* v2, long* sxy0, long* sxy1,
                 long* sxy2, long* p, long* flag) {
    NOT_IMPLEMENTED;
    return 0;
}
