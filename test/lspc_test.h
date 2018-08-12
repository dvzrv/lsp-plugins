#include <dsp/endian.h>
#include <core/debug.h>

#include <core/files/LSPCFile.h>

namespace lspc_test
{
    using namespace lsp;

    int test(int argc, const char **argv)
    {
        LSPCFile fd;

        //-----------------------------------------------------------------------------
        // Create LSPC file
        //-----------------------------------------------------------------------------
        fd.create("test_data/test.lspc");

        // Write audio chunk
        LSPCChunkWriter *wr = fd.write_chunk(LSPC_CHUNK_AUDIO);

        lspc_chunk_audio_header_t ahdr;
        memset(&ahdr, 0, sizeof(lspc_chunk_audio_header_t));

        ahdr.version        = 1;
        ahdr.channels       = 1;
        ahdr.sample_format  = LSPC_SAMPLE_FMT_F32LE;
        ahdr.sample_rate    = 48000;
        ahdr.codec          = LSPC_CODEC_PCM;
        ahdr.frames         = 48000;

        ahdr.version        = CPU_TO_BE(ahdr.version);
        ahdr.channels       = CPU_TO_BE(ahdr.channels);
        ahdr.sample_format  = CPU_TO_BE(ahdr.sample_format);
        ahdr.sample_rate    = CPU_TO_BE(ahdr.sample_rate);
        ahdr.codec          = CPU_TO_BE(ahdr.codec);
        ahdr.frames         = CPU_TO_BE(ahdr.frames);

        wr->write(&ahdr, sizeof(ahdr));
        size_t chunk_id     = wr->unique_id();
        float frame;
        for (size_t i=0; i<48000; ++i)
        {
            frame = sinf(2 * M_PI * i / 48000);
            frame = LE_TO_CPU(frame);
            wr->write(&frame, sizeof(frame));
        }
        wr->close();
        delete wr;

        // Write profile chunk
        wr                  = fd.write_chunk(LSPC_CHUNK_PROFILE);

        lspc_chunk_audio_profile_t prof;
        memset(&prof, 0, sizeof(lspc_chunk_audio_profile_t));

        prof.version        = 1;
        prof.chunk_id       = chunk_id;
        prof.chirp_order    = 1;
        prof.alpha          = 1.0;
        prof.beta           = 2.0;
        prof.gamma          = 3.0;
        prof.delta          = 4.0;
        prof.initial_freq   = 10.0;
        prof.final_freq     = 48000.0;

        prof.version        = CPU_TO_BE(prof.version);
        prof.chunk_id       = CPU_TO_BE(prof.chunk_id);
        prof.chirp_order    = CPU_TO_BE(prof.chirp_order);
        prof.alpha          = CPU_TO_BE(prof.alpha);
        prof.beta           = CPU_TO_BE(prof.beta);
        prof.gamma          = CPU_TO_BE(prof.gamma);
        prof.delta          = CPU_TO_BE(prof.delta);
        prof.initial_freq   = CPU_TO_BE(prof.initial_freq);
        prof.final_freq     = CPU_TO_BE(prof.final_freq);

        wr->write(&prof, sizeof(lspc_chunk_audio_profile_t));
        wr->close();

        fd.close();

        //-----------------------------------------------------------------------------
        // Read LSPC file
        //-----------------------------------------------------------------------------
        fd.open("test_data/test.lspc");

        for (chunk_id=1; ; chunk_id++)
        {
            // Get chunk
            LSPCChunkReader    *rd = fd.read_chunk(chunk_id);
            if (rd == NULL)
                break;

            // Check chunk type
            if (rd->magic() == LSPC_CHUNK_PROFILE)
            {
                ssize_t n = rd->read(&prof, sizeof(lspc_chunk_audio_profile_t));
                if (n != sizeof(lspc_chunk_audio_profile_t))
                    continue;
                lsp_trace("version = %d", int(BE_TO_CPU(prof.version)));
                lsp_trace("chunk_id = %d", int(BE_TO_CPU(prof.chunk_id)));
                lsp_trace("chirp_order = %d", int(BE_TO_CPU(prof.chirp_order)));
                lsp_trace("alpha = %f", BE_TO_CPU(prof.alpha));
                lsp_trace("beta = %f", BE_TO_CPU(prof.beta));
                lsp_trace("gamma = %f", BE_TO_CPU(prof.gamma));
                lsp_trace("delta = %f", BE_TO_CPU(prof.delta));
                lsp_trace("initial_freq = %f", BE_TO_CPU(prof.initial_freq));
                lsp_trace("final_freq = %f", BE_TO_CPU(prof.final_freq));

                rd->close();

                // Read data
                rd      = fd.read_chunk(BE_TO_CPU(prof.chunk_id));
                if (rd == NULL)
                    continue;

                n = rd->read(&ahdr, sizeof(lspc_chunk_audio_header_t));
                if (n != sizeof(lspc_chunk_audio_header_t))
                {
                    rd->close();
                    continue;
                }

                lsp_trace("version = %d", int(BE_TO_CPU(ahdr.version)));
                lsp_trace("channels = %d", int(BE_TO_CPU(ahdr.channels)));
                lsp_trace("sample_format = %d", int(BE_TO_CPU(ahdr.sample_format)));
                lsp_trace("sample_rate = %d", int(BE_TO_CPU(ahdr.sample_rate)));
                lsp_trace("codec = %d", int(BE_TO_CPU(ahdr.codec)));
                lsp_trace("frames = %d", int(BE_TO_CPU(ahdr.frames)));
                size_t k =BE_TO_CPU(ahdr.frames);

                for (size_t i=0; i<k; ++i)
                {
                    ssize_t k = rd->read(&frame, sizeof(frame));
                    if (k != sizeof(frame))
                    {
                        rd->close();
                        lsp_trace("Audio data corrupted");
                        continue;
                    }
                }
                lsp_trace("All audio frames have been successful read");
            }

            rd->close();
        }

        fd.close();

        return 0;
    }
}
