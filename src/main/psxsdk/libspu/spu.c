#include "common.h"
#include "libspu_internal.h"

#define WASTE_TIME()                                                           \
    sp4 = 0xD;                                                                 \
    for (sp0 = 0; sp0 < 0xF0; sp0++) {                                         \
        sp4 *= 3;                                                              \
    }

s32 _spu_reset(void) {
    volatile s32 sp0;
    volatile s32 sp4;
    u16 temp_a0;
    volatile s16* spucnt = &_spu_RXX->rxx.spucnt;

    temp_a0 = _spu_RXX->rxx.spucnt;
    *spucnt = temp_a0 & 0x7FCF;
    WASTE_TIME();
    temp_a0 &= 0xFFCF;
    _spu_RXX->rxx.spucnt = temp_a0;
    return 0;
}

INCLUDE_ASM("main/nonmatchings/psxsdk/libspu/spu", _spu_init);

INCLUDE_ASM("main/nonmatchings/psxsdk/libspu/spu", _spu_writeByIO);

void _spu_FiDMA(void) {
    volatile s32 sp0;
    volatile s32 sp4;
    s32 var_v1;
    volatile SPU_RXX* rxx;

    if (D_80033550 == 0) {
        WASTE_TIME();
        WASTE_TIME();
        WASTE_TIME();
    }

    rxx = &_spu_RXX->rxx;
    rxx->spucnt &= 0xFFCF;

    for (var_v1 = 0; rxx->spucnt & 0x30; var_v1++) {
        if (var_v1 + 1 > 0xF00) {
            break;
        }
    }

    if (_spu_transferCallback) {
        _spu_transferCallback();
        return;
    }
    DeliverEvent(0xF0000009U, 0x20U);
}

extern s32* D_80033508;
extern s32* D_8003350C;
extern s32* D_80033510;
extern s32* D_80033518;
extern s32 D_80033550;

void _spu_r_(s32 arg0, u16 arg1, s32 arg2) {
    volatile s32 sp0;
    volatile s32 sp4;
    _spu_RXX->rxx.trans_addr = arg1;
    WASTE_TIME();
    WASTE_TIME();

    _spu_RXX->rxx.spucnt |= 0x30;
    do {
        WASTE_TIME();
    } while (0);

    WASTE_TIME();
    *D_80033518 = (*D_80033518 & 0xF0FFFFFF) | 0x22000000;
    *D_80033508 = arg0;
    *D_8003350C = (arg2 << 0x10) | 0x10;
    D_80033550 = 1;
    *D_80033510 = 0x01000200;
}

INCLUDE_ASM("main/nonmatchings/psxsdk/libspu/spu", _spu_t);

s32 _spu_write(u32 arg0, u32 arg1) {

    if (_spu_transMode != 0) {
        _spu_writeByIO(arg0, arg1);
        return arg1;
    }
    _spu_t(2, _spu_tsa << _spu_mem_mode_plus);
    _spu_t(1);
    _spu_t(3, arg0, arg1);
    return arg1;
}

s32 _spu_read(s32 arg0, s32 arg1) {
    s32 temp = _spu_tsa << _spu_mem_mode_plus;
    _spu_t(2, temp);
    _spu_t(0);
    _spu_t(3, arg0, arg1);
    return arg1;
}

void _spu_FsetRXX(s32 arg0, u32 arg1, s32 arg2) {
    if (arg2 == 0) {
        _spu_RXX->raw[arg0] = arg1;
        return;
    }

    _spu_RXX->raw[arg0] = (arg1 >> _spu_mem_mode_plus);
}

INCLUDE_ASM("main/nonmatchings/psxsdk/libspu/spu", _spu_FsetRXXa);

INCLUDE_ASM("main/nonmatchings/psxsdk/libspu/spu", _spu_FgetRXXa);
