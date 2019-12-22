/*
 * fmop_k3.h
 *
 *  Created on: 21 нояб. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_AVX_PMATH_FMOP_KX_H_
#define DSP_ARCH_X86_AVX_PMATH_FMOP_KX_H_

#ifndef DSP_ARCH_X86_AVX_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_AVX_IMPL */

namespace avx
{
    #define OP_DSEL(a, b)   a
    #define OP_RSEL(a, b)   b

    #define FMADDSUB_K3_CORE(DST, SRC1, SRC2, OP) \
        __ASM_EMIT("xor         %[off], %[off]") \
        __ASM_EMIT("vbroadcastss %[k], %%ymm0") \
        __ASM_EMIT("sub         $32, %[count]") \
        __ASM_EMIT("vmovaps     %%ymm0, %%ymm1") \
        __ASM_EMIT("jb          2f")    \
        /* 32x blocks */ \
        __ASM_EMIT("1:") \
        __ASM_EMIT("vmovups     0x000(%[" SRC1 "], %[off]), %%ymm4") \
        __ASM_EMIT("vmovups     0x020(%[" SRC1 "], %[off]), %%ymm5") \
        __ASM_EMIT("vmovups     0x040(%[" SRC1 "], %[off]), %%ymm6") \
        __ASM_EMIT("vmovups     0x060(%[" SRC1 "], %[off]), %%ymm7") \
        __ASM_EMIT(OP "ps       0x000(%[" SRC2 "], %[off]), %%ymm0, %%ymm4") \
        __ASM_EMIT(OP "ps       0x020(%[" SRC2 "], %[off]), %%ymm1, %%ymm5") \
        __ASM_EMIT(OP "ps       0x040(%[" SRC2 "], %[off]), %%ymm0, %%ymm6") \
        __ASM_EMIT(OP "ps       0x060(%[" SRC2 "], %[off]), %%ymm1, %%ymm7") \
        __ASM_EMIT("vmovups     %%ymm4, 0x000(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%ymm5, 0x020(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%ymm6, 0x040(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%ymm7, 0x060(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x80, %[off]") \
        __ASM_EMIT("sub         $32, %[count]") \
        __ASM_EMIT("jae         1b") \
        /* 16x block */ \
        __ASM_EMIT("2:") \
        __ASM_EMIT("add         $16, %[count]")          /* 32 - 16 */ \
        __ASM_EMIT("jl          4f") \
        __ASM_EMIT("vmovups     0x000(%[" SRC1 "], %[off]), %%ymm4") \
        __ASM_EMIT("vmovups     0x020(%[" SRC1 "], %[off]), %%ymm5") \
        __ASM_EMIT(OP "ps       0x000(%[" SRC2 "], %[off]), %%ymm0, %%ymm4") \
        __ASM_EMIT(OP "ps       0x020(%[" SRC2 "], %[off]), %%ymm1, %%ymm5") \
        __ASM_EMIT("vmovups     %%ymm4, 0x000(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%ymm5, 0x020(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x40, %[off]") \
        __ASM_EMIT("sub         $16, %[count]") \
        /* 8x block */ \
        __ASM_EMIT("4:") \
        __ASM_EMIT("add         $8, %[count]")          /* 16 - 8 */ \
        __ASM_EMIT("jl          6f") \
        __ASM_EMIT("vmovups     0x000(%[" SRC1 "], %[off]), %%ymm4") \
        __ASM_EMIT(OP "ps       0x000(%[" SRC2 "], %[off]), %%ymm0, %%ymm4") \
        __ASM_EMIT("vmovups     %%ymm4, 0x000(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x20, %[off]") \
        __ASM_EMIT("sub         $8, %[count]") \
        \
        /* 4x block */ \
        __ASM_EMIT("6:") \
        __ASM_EMIT("add         $4, %[count]")          /* 8 - 4 */ \
        __ASM_EMIT("jl          8f") \
        __ASM_EMIT("vmovups     0x000(%[" SRC1 "], %[off]), %%xmm4") \
        __ASM_EMIT(OP "ps       0x000(%[" SRC2 "], %[off]), %%xmm0, %%xmm4") \
        __ASM_EMIT("vmovups     %%xmm4, 0x000(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x10, %[off]") \
        __ASM_EMIT("sub         $4, %[count]") \
        \
        /* 1x blocks */ \
        __ASM_EMIT("8:") \
        __ASM_EMIT("add         $3, %[count]")          /* 8 - 4 */ \
        __ASM_EMIT("jl          10f") \
        __ASM_EMIT("9:") \
        __ASM_EMIT("vmovss      0x000(%[" SRC1 "], %[off]), %%xmm4") \
        __ASM_EMIT(OP "ss       0x000(%[" SRC2 "], %[off]), %%xmm0, %%xmm4") \
        __ASM_EMIT("vmovss      %%xmm4, 0x000(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x04, %[off]") \
        __ASM_EMIT("dec         %[count]") \
        __ASM_EMIT("jge         9b") \
        __ASM_EMIT("10:")

    void fmadd_k3_fma3(float *dst, const float *src, float k, size_t count)
    {
        IF_ARCH_X86_64(size_t off);
        ARCH_X86_64_ASM
        (
            FMADDSUB_K3_CORE("dst", "dst", "src", "vfmadd231")
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r"(dst), [src] "r"(src),
              [k] "o" (k)
            : "cc", "memory",
              "%xmm0", "%xmm1",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void fmsub_k3_fma3(float *dst, const float *src, float k, size_t count)
    {
        IF_ARCH_X86_64(size_t off);
        ARCH_X86_64_ASM
        (
            FMADDSUB_K3_CORE("dst", "dst", "src", "vfnmadd231")
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r"(dst), [src] "r"(src),
              [k] "o" (k)
            : "cc", "memory",
              "%xmm0", "%xmm1",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void fmrsub_k3_fma3(float *dst, const float *src, float k, size_t count)
    {
        IF_ARCH_X86_64(size_t off);
        ARCH_X86_64_ASM
        (
            FMADDSUB_K3_CORE("dst", "dst", "src", "vfmsub231")
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r"(dst), [src] "r"(src),
              [k] "o" (k)
            : "cc", "memory",
              "%xmm0", "%xmm1",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void fmadd_k4_fma3(float *dst, const float *src1, const float *src2, float k, size_t count)
    {
        IF_ARCH_X86_64(size_t off);
        ARCH_X86_64_ASM
        (
            FMADDSUB_K3_CORE("dst", "src1", "src2", "vfmadd231")
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r"(dst), [src1] "r" (src1), [src2] "r" (src2),
              [k] "o" (k)
            : "cc", "memory",
              "%xmm0", "%xmm1",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void fmsub_k4_fma3(float *dst, const float *src1, const float *src2, float k, size_t count)
    {
        IF_ARCH_X86_64(size_t off);
        ARCH_X86_64_ASM
        (
            FMADDSUB_K3_CORE("dst", "src1", "src2", "vfnmadd231")
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r"(dst), [src1] "r" (src1), [src2] "r" (src2),
              [k] "o" (k)
            : "cc", "memory",
              "%xmm0", "%xmm1",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void fmrsub_k4_fma3(float *dst, const float *src1, const float *src2, float k, size_t count)
    {
        IF_ARCH_X86_64(size_t off);
        ARCH_X86_64_ASM
        (
            FMADDSUB_K3_CORE("dst", "src1", "src2", "vfmsub231")
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r"(dst), [src1] "r" (src1), [src2] "r" (src2),
              [k] "o" (k)
            : "cc", "memory",
              "%xmm0", "%xmm1",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    #undef FMADDSUB_K3_CORE

    #define FMOP_K4_CORE(DST, SRC1, SRC2, OP, SEL) \
        __ASM_EMIT("xor         %[off], %[off]") \
        __ASM_EMIT("vbroadcastss %[k], %%ymm0") \
        __ASM_EMIT("sub         $24, %[count]") \
        __ASM_EMIT("vmovaps     %%ymm0, %%ymm1") \
        __ASM_EMIT("jb          2f")    \
        /* 24x blocks */ \
        __ASM_EMIT("1:") \
        __ASM_EMIT("vmovups     0x000(%[" SRC1 "], %[off]), %%ymm5") \
        __ASM_EMIT("vmovups     0x020(%[" SRC1 "], %[off]), %%ymm6") \
        __ASM_EMIT("vmovups     0x040(%[" SRC1 "], %[off]), %%ymm7") \
        __ASM_EMIT("vmulps      0x000(%[" SRC2 "], %[off]), %%ymm0, %%ymm2") \
        __ASM_EMIT("vmulps      0x020(%[" SRC2 "], %[off]), %%ymm1, %%ymm3") \
        __ASM_EMIT("vmulps      0x040(%[" SRC2 "], %[off]), %%ymm0, %%ymm4") \
        __ASM_EMIT(OP "ps       " SEL("%%ymm2", "%%ymm5") ", "  SEL("%%ymm5", "%%ymm2") ", %%ymm5") \
        __ASM_EMIT(OP "ps       " SEL("%%ymm3", "%%ymm6") ", "  SEL("%%ymm6", "%%ymm3") ", %%ymm6") \
        __ASM_EMIT(OP "ps       " SEL("%%ymm4", "%%ymm7") ", "  SEL("%%ymm7", "%%ymm4") ", %%ymm7") \
        __ASM_EMIT("vmovups     %%ymm5, 0x000(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%ymm6, 0x020(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%ymm7, 0x040(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x60, %[off]") \
        __ASM_EMIT("sub         $24, %[count]") \
        __ASM_EMIT("jae         1b") \
        /* 16x block */ \
        __ASM_EMIT("2:") \
        __ASM_EMIT("add         $8, %[count]")          /* 24 - 16 */ \
        __ASM_EMIT("jl          4f") \
        __ASM_EMIT("vmovups     0x000(%[" SRC1 "], %[off]), %%ymm6") \
        __ASM_EMIT("vmovups     0x020(%[" SRC1 "], %[off]), %%ymm7") \
        __ASM_EMIT("vmulps      0x000(%[" SRC2 "], %[off]), %%ymm0, %%ymm2") \
        __ASM_EMIT("vmulps      0x020(%[" SRC2 "], %[off]), %%ymm1, %%ymm3") \
        __ASM_EMIT(OP "ps       " SEL("%%ymm2", "%%ymm6") ", "  SEL("%%ymm6", "%%ymm2") ", %%ymm6") \
        __ASM_EMIT(OP "ps       " SEL("%%ymm3", "%%ymm7") ", "  SEL("%%ymm7", "%%ymm3") ", %%ymm7") \
        __ASM_EMIT("vmovups     %%ymm6, 0x000(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%ymm7, 0x020(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x40, %[off]") \
        __ASM_EMIT("sub         $16, %[count]") \
        /* 8x block */ \
        __ASM_EMIT("4:") \
        __ASM_EMIT("add         $8, %[count]")          /* 16 - 8 */ \
        __ASM_EMIT("jl          6f") \
        __ASM_EMIT("vmovups     0x000(%[" SRC1 "], %[off]), %%ymm6") \
        __ASM_EMIT("vmulps      0x000(%[" SRC2 "], %[off]), %%ymm0, %%ymm2") \
        __ASM_EMIT(OP "ps       " SEL("%%ymm2", "%%ymm6") ", "  SEL("%%ymm6", "%%ymm2") ", %%ymm6") \
        __ASM_EMIT("vmovups     %%ymm6, 0x000(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x20, %[off]") \
        __ASM_EMIT("sub         $8, %[count]") \
        \
        /* 4x block */ \
        __ASM_EMIT("6:") \
        __ASM_EMIT("add         $4, %[count]")          /* 8 - 4 */ \
        __ASM_EMIT("jl          8f") \
        __ASM_EMIT("vmovups     0x000(%[" SRC1 "], %[off]), %%xmm6") \
        __ASM_EMIT("vmulps      0x000(%[" SRC2 "], %[off]), %%xmm0, %%xmm2") \
        __ASM_EMIT(OP "ps       " SEL("%%xmm2", "%%xmm6") ", "  SEL("%%xmm6", "%%xmm2") ", %%xmm6") \
        __ASM_EMIT("vmovups     %%xmm6, 0x000(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x10, %[off]") \
        __ASM_EMIT("sub         $4, %[count]") \
        \
        /* 1x blocks */ \
        __ASM_EMIT("8:") \
        __ASM_EMIT("add         $3, %[count]")          /* 8 - 4 */ \
        __ASM_EMIT("jl          10f") \
        __ASM_EMIT("9:") \
        __ASM_EMIT("vmovss      0x000(%[" SRC1 "], %[off]), %%xmm6") \
        __ASM_EMIT("vmulss      0x000(%[" SRC2 "], %[off]), %%xmm0, %%xmm2") \
        __ASM_EMIT(OP "ss       " SEL("%%xmm2", "%%xmm6") ", "  SEL("%%xmm6", "%%xmm2") ", %%xmm6") \
        __ASM_EMIT("vmovss      %%xmm6, 0x000(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x04, %[off]") \
        __ASM_EMIT("dec         %[count]") \
        __ASM_EMIT("jge         9b") \
        __ASM_EMIT("10:")

    void fmadd_k3(float *dst, const float *src, float k, size_t count)
    {
        IF_ARCH_X86_64(size_t off);
        ARCH_X86_64_ASM
        (
            FMOP_K4_CORE("dst", "dst", "src", "vadd", OP_DSEL)
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r"(dst), [src] "r"(src),
              [k] "o" (k)
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void fmsub_k3(float *dst, const float *src, float k, size_t count)
    {
        IF_ARCH_X86_64(size_t off);
        ARCH_X86_64_ASM
        (
            FMOP_K4_CORE("dst", "dst", "src", "vsub", OP_DSEL)
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r"(dst), [src] "r"(src),
              [k] "o" (k)
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void fmrsub_k3(float *dst, const float *src, float k, size_t count)
    {
        IF_ARCH_X86_64(size_t off);
        ARCH_X86_64_ASM
        (
            FMOP_K4_CORE("dst", "dst", "src", "vsub", OP_RSEL)
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r"(dst), [src] "r"(src),
              [k] "o" (k)
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void fmmul_k3(float *dst, const float *src, float k, size_t count)
    {
        IF_ARCH_X86_64(size_t off);
        ARCH_X86_64_ASM
        (
            FMOP_K4_CORE("dst", "dst", "src", "vmul", OP_DSEL)
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r"(dst), [src] "r"(src),
              [k] "o" (k)
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void fmdiv_k3(float *dst, const float *src, float k, size_t count)
    {
        IF_ARCH_X86_64(size_t off);
        ARCH_X86_64_ASM
        (
            FMOP_K4_CORE("dst", "dst", "src", "vdiv", OP_DSEL)
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r"(dst), [src] "r"(src),
              [k] "o" (k)
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void fmrdiv_k3(float *dst, const float *src, float k, size_t count)
    {
        IF_ARCH_X86_64(size_t off);
        ARCH_X86_64_ASM
        (
            FMOP_K4_CORE("dst", "dst", "src", "vdiv", OP_RSEL)
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r"(dst), [src] "r"(src),
              [k] "o" (k)
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void fmadd_k4(float *dst, const float *src1, const float *src2, float k, size_t count)
    {
        IF_ARCH_X86_64(size_t off);
        ARCH_X86_64_ASM
        (
            FMOP_K4_CORE("dst", "src1", "src2", "vadd", OP_DSEL)
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r"(dst), [src1] "r" (src1), [src2] "r" (src2),
              [k] "o" (k)
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void fmsub_k4(float *dst, const float *src1, const float *src2, float k, size_t count)
    {
        IF_ARCH_X86_64(size_t off);
        ARCH_X86_64_ASM
        (
            FMOP_K4_CORE("dst", "src1", "src2", "vsub", OP_DSEL)
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r"(dst), [src1] "r" (src1), [src2] "r" (src2),
              [k] "o" (k)
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void fmrsub_k4(float *dst, const float *src1, const float *src2, float k, size_t count)
    {
        IF_ARCH_X86_64(size_t off);
        ARCH_X86_64_ASM
        (
            FMOP_K4_CORE("dst", "src1", "src2", "vsub", OP_RSEL)
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r"(dst), [src1] "r" (src1), [src2] "r" (src2),
              [k] "o" (k)
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void fmmul_k4(float *dst, const float *src1, const float *src2, float k, size_t count)
    {
        IF_ARCH_X86_64(size_t off);
        ARCH_X86_64_ASM
        (
            FMOP_K4_CORE("dst", "src1", "src2", "vmul", OP_DSEL)
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r"(dst), [src1] "r" (src1), [src2] "r" (src2),
              [k] "o" (k)
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void fmdiv_k4(float *dst, const float *src1, const float *src2, float k, size_t count)
    {
        IF_ARCH_X86_64(size_t off);
        ARCH_X86_64_ASM
        (
            FMOP_K4_CORE("dst", "src1", "src2", "vdiv", OP_DSEL)
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r"(dst), [src1] "r" (src1), [src2] "r" (src2),
              [k] "o" (k)
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void fmrdiv_k4(float *dst, const float *src1, const float *src2, float k, size_t count)
    {
        IF_ARCH_X86_64(size_t off);
        ARCH_X86_64_ASM
        (
            FMOP_K4_CORE("dst", "src1", "src2", "vdiv", OP_RSEL)
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r"(dst), [src1] "r" (src1), [src2] "r" (src2),
              [k] "o" (k)
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    #undef FMOP_K4_CORE


    #undef OP_DSEL
    #undef OP_RSEL
}

#endif /* DSP_ARCH_X86_AVX_PMATH_FMOP_KX_H_ */
