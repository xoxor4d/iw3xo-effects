/***************************************************************************************************

  Zyan Disassembler Library (Zydis)

  Original Author : Joel Hoener

 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.

***************************************************************************************************/

/**
 * @file
 *
 * This file implements a tool that is supposed to be fed as input for fuzzers like AFL, reading a
 * control block from `stdin`, allowing the fuzzer to reach every possible code-path, testing any
 * possible combination of disassembler configurations.
 */

#include <stdio.h>
#include <Zycore/LibC.h>
#include <Zydis/Zydis.h>

#ifdef ZYAN_WINDOWS
#   include <fcntl.h>
#   include <io.h>
#endif

/* ============================================================================================== */
/* Enums and types                                                                                */
/* ============================================================================================== */

/**
 * @brief   Defines the `ZydisFuzzControlBlock` struct.
 */
typedef struct ZydisFuzzControlBlock_
{
    ZydisMachineMode machine_mode;
    ZydisAddressWidth address_width;
    ZyanBool decoder_mode[ZYDIS_DECODER_MODE_MAX_VALUE + 1];
    ZydisFormatterStyle formatter_style;
    ZyanUPointer formatter_properties[ZYDIS_FORMATTER_PROP_MAX_VALUE + 1];
    char string[16];
} ZydisFuzzControlBlock;

/* ============================================================================================== */
/* Macros                                                                                         */
/* ============================================================================================== */

// Limit maximum amount of bytes
#define ZYDIS_FUZZ_MAX_BYTES (1024 * 10 /* 10 KiB */)

#ifdef ZYDIS_FUZZ_AFL_FAST
#   define ZYDIS_MAYBE_FPUTS(x, y)
#else
#   define ZYDIS_MAYBE_FPUTS(x, y) fputs(x, y)
#endif

/* ============================================================================================== */
/* Main iteration                                                                                 */
/* ============================================================================================== */

static int DoIteration(void)
{
    ZydisFuzzControlBlock control_block;

#ifdef ZYAN_WINDOWS
    // The `stdin` pipe uses text-mode on Windows platforms by default. We need it to be opened in
    // binary mode
    _setmode(_fileno(ZYAN_STDIN), _O_BINARY);
#endif

    if (fread(&control_block, 1, sizeof(control_block), ZYAN_STDIN) != sizeof(control_block))
    {
        ZYDIS_MAYBE_FPUTS("Not enough bytes to fuzz\n", ZYAN_STDERR);
        return EXIT_FAILURE;
    }
    control_block.string[ZYAN_ARRAY_LENGTH(control_block.string) - 1] = 0;

    ZydisDecoder decoder;
    if (!ZYAN_SUCCESS(ZydisDecoderInit(&decoder, control_block.machine_mode,
        control_block.address_width)))
    {
        ZYDIS_MAYBE_FPUTS("Failed to initialize decoder\n", ZYAN_STDERR);
        return EXIT_FAILURE;
    }
    for (ZydisDecoderMode mode = 0; mode <= ZYDIS_DECODER_MODE_MAX_VALUE; ++mode)
    {
        if (!ZYAN_SUCCESS(
            ZydisDecoderEnableMode(&decoder, mode, control_block.decoder_mode[mode] ? 1 : 0)))
        {
            ZYDIS_MAYBE_FPUTS("Failed to adjust decoder-mode\n", ZYAN_STDERR);
            return EXIT_FAILURE;
        }
    }

    ZydisFormatter formatter;
    if (!ZYAN_SUCCESS(ZydisFormatterInit(&formatter, control_block.formatter_style)))
    {
        ZYDIS_MAYBE_FPUTS("Failed to initialize formatter\n", ZYAN_STDERR);
        return EXIT_FAILURE;
    }
    for (ZydisFormatterProperty prop = 0; prop <= ZYDIS_FORMATTER_PROP_MAX_VALUE; ++prop)
    {
        switch (prop)
        {
        case ZYDIS_FORMATTER_PROP_DEC_PREFIX:
        case ZYDIS_FORMATTER_PROP_DEC_SUFFIX:
        case ZYDIS_FORMATTER_PROP_HEX_PREFIX:
        case ZYDIS_FORMATTER_PROP_HEX_SUFFIX:
            control_block.formatter_properties[prop] =
                control_block.formatter_properties[prop] ? (ZyanUPointer)&control_block.string : 0;
            break;
        default:
            break;
        }
        if (!ZYAN_SUCCESS(ZydisFormatterSetProperty(&formatter, prop,
            control_block.formatter_properties[prop])))
        {
            ZYDIS_MAYBE_FPUTS("Failed to set formatter-attribute\n", ZYAN_STDERR);
            return EXIT_FAILURE;
        }
    }

    ZyanU8 buffer[1024];
    ZyanUSize buffer_size;
    ZyanUSize buffer_remaining = 0;
    ZyanUSize read_offset_base = 0;
    do
    {
        buffer_size = fread(buffer + buffer_remaining, 1, sizeof(buffer) - buffer_remaining,
            ZYAN_STDIN);
        if (buffer_size != (sizeof(buffer) - buffer_remaining))
        {
            if (ferror(ZYAN_STDIN))
            {
                return EXIT_FAILURE;
            }
            ZYAN_ASSERT(feof(ZYAN_STDIN));
        }
        buffer_size += buffer_remaining;

        ZydisDecodedInstruction instruction;
        ZyanStatus status;
        ZyanUSize read_offset = 0;
        char format_buffer[256];

        while ((status = ZydisDecoderDecodeBuffer(&decoder, buffer + read_offset,
            buffer_size - read_offset, &instruction)) != ZYDIS_STATUS_NO_MORE_DATA)
        {
            const ZyanU64 runtime_address = read_offset_base + read_offset;

            if (!ZYAN_SUCCESS(status))
            {
                ++read_offset;
                continue;
            }
           
            ZydisFormatterFormatInstruction(&formatter, &instruction, format_buffer,
                sizeof(format_buffer), runtime_address);
            read_offset += instruction.length;
        }

        buffer_remaining = 0;
        if (read_offset < sizeof(buffer))
        {
            buffer_remaining = sizeof(buffer) - read_offset;
            ZYAN_MEMMOVE(buffer, buffer + read_offset, buffer_remaining);
        }
        read_offset_base += read_offset;
    } while (buffer_size == sizeof(buffer) && read_offset_base < ZYDIS_FUZZ_MAX_BYTES);

    return EXIT_SUCCESS;
}

/* ============================================================================================== */
/* Entry point                                                                                    */
/* ============================================================================================== */

int main(void)
{
    if (ZydisGetVersion() != ZYDIS_VERSION)
    {
        fputs("Invalid zydis version\n", ZYAN_STDERR);
        return EXIT_FAILURE;
    }

#ifdef ZYDIS_FUZZ_AFL_FAST
    while (__AFL_LOOP(1000))
    {
        DoIteration();
    }
    return EXIT_SUCCESS;
#else
    return DoIteration();
#endif
}

/* ============================================================================================== */

