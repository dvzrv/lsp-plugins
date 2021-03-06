/*
 * avx2.cpp
 *
 *  Created on: 17 дек. 2018 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/test.h>

#include <core/types.h>
#include <core/debug.h>

#include <dsp/arch/x86/features.h>

#define DSP_ARCH_X86_AVX2_IMPL

#include <dsp/arch/x86/avx2/pmath/exp.h>
#include <dsp/arch/x86/avx2/pmath/log.h>
#include <dsp/arch/x86/avx2/pmath/pow.h>

#include <dsp/arch/x86/avx2/search/iminmax.h>

#include <dsp/arch/x86/avx2/graphics/transpose.h>
#include <dsp/arch/x86/avx2/graphics/effects.h>

#undef DSP_ARCH_X86_AVX2_IMPL

namespace avx2
{
    using namespace x86;

    #define EXPORT2(function, export)               { dsp::function = avx2::export; TEST_EXPORT(avx2::export); }
    #define EXPORT1(function)                       EXPORT2(function, function)

    #define EXPORT2_X64(function, export)           IF_ARCH_X86_64(EXPORT2(function, export));
    #define SUPPORT_X64(function)                   IF_ARCH_X86_64(TEST_EXPORT(avx2::function))

    #define CEXPORT2(cond, function, export)    \
    IF_ARCH_X86( \
            TEST_EXPORT(avx2::export); \
            if (cond) \
                dsp::function = avx2::export; \
        );

    #define CEXPORT1(cond, export)    \
    IF_ARCH_X86( \
            TEST_EXPORT(avx2::export); \
            if (cond) \
                dsp::export = avx2::export; \
        );

    #define CEXPORT2_X64(cond, function, export)    \
        IF_ARCH_X86_64( \
                TEST_EXPORT(avx2::export); \
                if (cond) \
                    dsp::function = avx2::export; \
            );

    #define CEXPORT1_X64(cond, export)    \
        IF_ARCH_X86_64( \
                TEST_EXPORT(avx2::export); \
                if (cond) \
                    dsp::export = avx2::export; \
            );

    void dsp_init(const cpu_features_t *f)
    {
        if ((f->features & (CPU_OPTION_AVX | CPU_OPTION_AVX2)) != (CPU_OPTION_AVX | CPU_OPTION_AVX2))
            return;

        lsp_trace("Optimizing DSP for AVX2 instruction set");

        bool favx   = feature_check(f, FEAT_FAST_AVX);

        CEXPORT2_X64(favx, exp1, x64_exp1);
        CEXPORT2_X64(favx, exp2, x64_exp2);

        CEXPORT2_X64(favx, logb1, x64_logb1);
        CEXPORT2_X64(favx, logb2, x64_logb2);
        CEXPORT2_X64(favx, loge1, x64_loge1);
        CEXPORT2_X64(favx, loge2, x64_loge2);
        CEXPORT2_X64(favx, logd1, x64_logd1);
        CEXPORT2_X64(favx, logd2, x64_logd2);

        CEXPORT2_X64(favx, powcv1, x64_powcv1);
        CEXPORT2_X64(favx, powcv2, x64_powcv2);
        CEXPORT2_X64(favx, powvc1, x64_powvc1);
        CEXPORT2_X64(favx, powvc2, x64_powvc2);
        CEXPORT2_X64(favx, powvx1, x64_powvx1);
        CEXPORT2_X64(favx, powvx2, x64_powvx2);

        CEXPORT2_X64(favx, eff_hsla_hue, x64_eff_hsla_hue);
        CEXPORT2_X64(favx, eff_hsla_sat, x64_eff_hsla_sat);
        CEXPORT2_X64(favx, eff_hsla_light, x64_eff_hsla_light);
        CEXPORT2_X64(favx, eff_hsla_alpha, x64_eff_hsla_alpha);

        CEXPORT1(favx, min_index);
        CEXPORT1(favx, max_index);
        CEXPORT1(favx, minmax_index);
        CEXPORT1(favx, abs_min_index);
        CEXPORT1(favx, abs_max_index);
        CEXPORT1(favx, abs_minmax_index);

        if (f->features & CPU_OPTION_FMA3)
        {
            CEXPORT2_X64(favx, exp1, x64_exp1_fma3);
            CEXPORT2_X64(favx, exp2, x64_exp2_fma3);

            CEXPORT2_X64(favx, logb1, x64_logb1_fma3);
            CEXPORT2_X64(favx, logb2, x64_logb2_fma3);
            CEXPORT2_X64(favx, loge1, x64_loge1_fma3);
            CEXPORT2_X64(favx, loge2, x64_loge2_fma3);
            CEXPORT2_X64(favx, logd1, x64_logd1_fma3);
            CEXPORT2_X64(favx, logd2, x64_logd2_fma3);

            CEXPORT2_X64(favx, powcv1, x64_powcv1_fma3);
            CEXPORT2_X64(favx, powcv2, x64_powcv2_fma3);
            CEXPORT2_X64(favx, powvc1, x64_powvc1_fma3);
            CEXPORT2_X64(favx, powvc2, x64_powvc2_fma3);
            CEXPORT2_X64(favx, powvx1, x64_powvx1_fma3);
            CEXPORT2_X64(favx, powvx2, x64_powvx2_fma3);
        }
    }
}
