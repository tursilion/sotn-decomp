#include "common.h"
#include "libsnd_internal.h"

INCLUDE_ASM("main/nonmatchings/psxsdk/libsnd/seqread", _SsSeqPlay);

INCLUDE_ASM("main/nonmatchings/psxsdk/libsnd/seqread", _SsGetSeqData);

void SpuVmKeyOff(s16, s16, u8, s32);
void SpuVmKeyOn(s16, s16, u8, s32, s32, s32);

void _SsNoteOn(s16 arg0, s16 arg1, s32 arg2, s32 arg3) {
    s32 temp_a0;
    s32 temp_s1;
    u8 temp_v1;
    u8 pan;
    struct SeqStruct* temp_s0;
    temp_s0 = &_ss_score[arg0][arg1];
    pan = arg3;
    temp_v1 = temp_s0->channel;
    temp_s1 = pan & 0xFF;
    pan = temp_s0->panpot[temp_v1];
    if ((!((temp_s0->padaa >> temp_v1) & 1)) && (temp_s0->unk74 != 0)) {
        if (arg3 & 0xFF) {
            SpuVmKeyOn(arg0 | (arg1 << 8), temp_s0->unk4c,
                       temp_s0->programs[temp_v1], arg2 & 0xFF, temp_s1, pan);
            temp_s0->padA6 = temp_s1;
            return;
        }
        SpuVmKeyOff(arg0 | (arg1 << 8), temp_s0->unk4c,
                    temp_s0->programs[temp_v1], arg2 & 0xFF);
    }
}

s32 _SsReadDeltaValue(s16, s16);

void _SsSetProgramChange(s16 arg0, s16 arg1, s8 arg2) {
    struct SeqStruct* temp_s0;
    temp_s0 = &_ss_score[arg0][arg1];
    temp_s0->programs[temp_s0->channel] = arg2;
    temp_s0->delta_value = _SsReadDeltaValue(arg0, arg1);
}

INCLUDE_ASM("main/nonmatchings/psxsdk/libsnd/seqread", _SsSetControlChange);

void _SsContModulation(s16 arg0, s16 arg1, u8 arg2) {
    ProgAtr sp10;
    VagAtr sp20;
    s32 toneNum;
    struct SeqStruct* temp_s2;
    u8 channel;

    temp_s2 = &_ss_score[arg0][arg1];
    channel = temp_s2->channel;
    SsUtGetProgAtr(temp_s2->unk4c, temp_s2->programs[channel], &sp10);
    for (toneNum = 0; toneNum < sp10.tones; toneNum++) {
        SsUtGetVagAtr(
            temp_s2->unk4c, temp_s2->programs[channel], toneNum, &sp20);
        sp20.vibW = arg2;
        SsUtSetVagAtr(
            temp_s2->unk4c, temp_s2->programs[channel], toneNum, &sp20);
    }
    temp_s2->delta_value = _SsReadDeltaValue(arg0, arg1);
}

void _SsContPortaTime(s16 arg0, s16 arg1, u8 arg2) {
    ProgAtr sp10;
    VagAtr sp20;
    s32 toneNum;
    struct SeqStruct* temp_s2;
    u8 channel;

    temp_s2 = &_ss_score[arg0][arg1];
    channel = temp_s2->channel;
    SsUtGetProgAtr(temp_s2->unk4c, temp_s2->programs[channel], &sp10);
    for (toneNum = 0; toneNum < sp10.tones; toneNum++) {
        SsUtGetVagAtr(
            temp_s2->unk4c, temp_s2->programs[channel], toneNum, &sp20);
        sp20.porT = arg2;
        SsUtSetVagAtr(
            temp_s2->unk4c, temp_s2->programs[channel], toneNum, &sp20);
    }
    temp_s2->delta_value = _SsReadDeltaValue(arg0, arg1);
}

INCLUDE_ASM("main/nonmatchings/psxsdk/libsnd/seqread", _SsContPortamento);

void _SsContResetAll(s16 arg0, s16 arg1) {
    struct SeqStruct* temp_s0;
    temp_s0 = &_ss_score[arg0][arg1];
    func_80021F0C();
    SpuVmDamperOff();
    temp_s0->programs[temp_s0->channel] = temp_s0->channel;
    temp_s0->unk13 = 0;
    temp_s0->unk14 = 0;
    temp_s0->vol[temp_s0->channel] = 0x7f;
    temp_s0->panpot[temp_s0->channel] = 64;
    temp_s0->delta_value = _SsReadDeltaValue(arg0, arg1);
}

INCLUDE_ASM("main/nonmatchings/psxsdk/libsnd/seqread", _SsContNrpn1);

INCLUDE_ASM("main/nonmatchings/psxsdk/libsnd/seqread", _SsContNrpn2);

void _SsContRpn1(s16 arg0, s16 arg1, u8 arg2) {
    struct SeqStruct* temp_s0;

    temp_s0 = &_ss_score[arg0][arg1];
    temp_s0->unk13 = arg2;
    temp_s0->unk29 += 1;
    temp_s0->delta_value = _SsReadDeltaValue(arg0, arg1);
}

void _SsContRpn2(s16 arg0, s16 arg1, u8 arg2) {
    struct SeqStruct* temp_s0;

    temp_s0 = &_ss_score[arg0][arg1];
    temp_s0->unk14 = arg2;
    temp_s0->unk29 += 1;
    temp_s0->delta_value = _SsReadDeltaValue(arg0, arg1);
}

INCLUDE_ASM("main/nonmatchings/psxsdk/libsnd/seqread", _SsContDataEntry);

INCLUDE_ASM("main/nonmatchings/psxsdk/libsnd/seqread", _SsSndSetVabAttr);

void SpuVmPitchBend(s32, s16, u8, u8);
s32 _SsReadDeltaValue(s16, s16);

void _SsSetPitchBend(s16 arg0, s16 arg1) {
    struct SeqStruct* temp_s0;
    u8* temp_v1;
    u8 temp;
    temp_s0 = &_ss_score[arg0][arg1];
    temp = temp_s0->channel;
    temp_v1 = temp_s0->read_pos++;
    SpuVmPitchBend(
        (arg1 << 8) | arg0, temp_s0->unk4c, temp_s0->programs[temp], *temp_v1);
    temp_s0->delta_value = _SsReadDeltaValue(arg0, arg1);
}

INCLUDE_ASM("main/nonmatchings/psxsdk/libsnd/seqread", _SsGetMetaEvent);

s32 _SsReadDeltaValue(s16 arg0, s16 arg1) {
    s32 temp_v0;
    s32 var_v0;
    u8 temp_v1_2;
    u32 var_a0;
    u8* temp_v1;
    struct SeqStruct* temp_a1 = &_ss_score[arg0][arg1];
    var_a0 = *temp_a1->read_pos++;
    if (var_a0 == 0) {
        return 0;
    }

    // variable length quantity decoding (midi)
    if (var_a0 & 0x80) {
        var_a0 &= 0x7F;
        do {
            temp_v1_2 = *temp_a1->read_pos++;
            var_a0 = (var_a0 << 7) + (temp_v1_2 & 0x7F);
        } while (temp_v1_2 & 0x80);
    }

    temp_v0 = var_a0 * 10;
    temp_a1->unk80 += temp_v0;
    return temp_v0;
}
