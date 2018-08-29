/*
 * simple2.cpp
 *
 *  Created on: 23 авг. 2018 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/utest.h>
#include <test/FloatBuffer.h>
#include <core/sugar.h>

#define MIN_RANK 8
#define MAX_RANK 16

namespace native
{
    void    scale2(float *dst, float k, size_t count);
    void    scale3(float *dst, const float *src, float k, size_t count);

    void    scale_add3(float *dst, const float *src, float k, size_t count);
    void    scale_sub3(float *dst, const float *src, float k, size_t count);
    void    scale_mul3(float *dst, const float *src, float k, size_t count);
    void    scale_div3(float *dst, const float *src, float k, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        void    scale2(float *dst, float k, size_t count);
        void    scale3(float *dst, const float *src, float k, size_t count);

        void    scale_add3(float *dst, const float *src, float k, size_t count);
        void    scale_sub3(float *dst, const float *src, float k, size_t count);
        void    scale_mul3(float *dst, const float *src, float k, size_t count);
        void    scale_div3(float *dst, const float *src, float k, size_t count);
    }
)

typedef void (* scale2)(float *dst, float k, size_t count);
typedef void (* scale3)(float *dst, const float *src, float k, size_t count);

//-----------------------------------------------------------------------------
// Unit test for complex multiplication
UTEST_BEGIN("dsp.pmath", scale)

    void call(const char *label, size_t align, scale2 func1, scale2 func2)
    {
        if (!UTEST_SUPPORTED(func1))
            return;
        if (!UTEST_SUPPORTED(func2))
            return;

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                32, 64, 65, 100, 999)
        {
            for (size_t mask=0; mask <= 0x01; ++mask)
            {
                printf("Testing %s on input buffer of %d numbers, mask=0x%x...\n", label, int(count), int(mask));

                FloatBuffer dst1(count, align, mask & 0x01);
                FloatBuffer dst2(dst1);

                // Call functions
                func1(dst1, 0.5f, count);
                func2(dst2, 0.5f, count);

                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                // Compare buffers
                if (!dst1.equals_absolute(dst2, 1e-5))
                {
                    dst1.dump("dst1");
                    dst2.dump("dst2");
                    UTEST_FAIL_MSG("Output of functions for test '%s' differs", label);
                }
            }
        }
    }

    void call(const char *label, size_t align, scale3 func1, scale3 func2)
    {
        if (!UTEST_SUPPORTED(func1))
            return;
        if (!UTEST_SUPPORTED(func2))
            return;

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                32, 64, 65, 100, 999)
        {
            for (size_t mask=0; mask <= 0x03; ++mask)
            {
                printf("Testing %s on input buffer of %d numbers, mask=0x%x...\n", label, int(count), int(mask));

                FloatBuffer src(count, align, mask & 0x01);
                FloatBuffer dst1(count, align, mask & 0x02);
                FloatBuffer dst2(dst1);

                // Call functions
                func1(dst1, src, 0.5f, count);
                func2(dst2, src, 0.5f, count);

                UTEST_ASSERT_MSG(src.valid(), "Source buffer corrupted");
                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                // Compare buffers
                if (!dst1.equals_absolute(dst2, 1e-5))
                {
                    src.dump("src1");
                    dst1.dump("dst1");
                    dst2.dump("dst2");
                    UTEST_FAIL_MSG("Output of functions for test '%s' differs", label);
                }
            }
        }
    }

    UTEST_MAIN
    {
        IF_ARCH_X86(call("scale2 sse", 16, native::scale2, sse::scale2));
        IF_ARCH_X86(call("scale3 sse", 16, native::scale3, sse::scale3));
        IF_ARCH_X86(call("scale_add3 sse", 16, native::scale_add3, sse::scale_add3));
        IF_ARCH_X86(call("scale_sub3 sse", 16, native::scale_sub3, sse::scale_sub3));
        IF_ARCH_X86(call("scale_mul3 sse", 16, native::scale_mul3, sse::scale_mul3));
        IF_ARCH_X86(call("scale_div3 sse", 16, native::scale_div3, sse::scale_div3));
    }
UTEST_END


