#include "common.h"
#include "libsnd_internal.h"

void SsSetSerialVol(u8 s_num, s16 voll, s16 volr) {
    SpuCommonAttr attr;
    if (s_num == 0) {
        attr.mask = 0xC0;
        if (voll >= 128) {
            voll = 127;
        }
        if (volr >= 128) {
            volr = 127;
        }
        attr.cd.volume.left = voll * 258;
        attr.cd.volume.right = volr * 258;
    }
    if (s_num == 1) {
        attr.mask = 0xC00;
        if (voll >= 128) {
            voll = 127;
        }
        if (volr >= 128) {
            volr = 127;
        }
        attr.ext.volume.left = voll * 258;
        attr.ext.volume.right = volr * 258;
    }
    SpuSetCommonAttr(&attr);
}
