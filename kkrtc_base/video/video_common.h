//
// Created by devyk on 2023/10/20.
//

#ifndef KKRTC_VIDEO_COMMON_H
#define KKRTC_VIDEO_COMMON_H

#include <stdint.h>

#include <string>
#include "system/kkrtc_export.h"
#include "utils/time_utils.h"

namespace cricket {

//////////////////////////////////////////////////////////////////////////////
// Definition of FourCC codes
//////////////////////////////////////////////////////////////////////////////
// Convert four characters to a FourCC code.
// Needs to be a macro otherwise the OS X compiler complains when the kFormat*
// constants are used in a switch.
#define CRICKET_FOURCC(a, b, c, d)                                \
  ((static_cast<uint32_t>(a)) | (static_cast<uint32_t>(b) << 8) | \
   (static_cast<uint32_t>(c) << 16) | (static_cast<uint32_t>(d) << 24))
// Some pages discussing FourCC codes:
//   http://www.fourcc.org/yuv.php
//   http://v4l2spec.bytesex.org/spec/book1.htm
//   http://developer.apple.com/quicktime/icefloe/dispatch020.html
//   http://msdn.microsoft.com/library/windows/desktop/dd206750.aspx#nv12
//   http://people.xiph.org/~xiphmont/containers/nut/nut4cc.txt

// FourCC codes grouped according to implementation efficiency.
// Primary formats should convert in 1 efficient step.
// Secondary formats are converted in 2 steps.
// Auxilliary formats call primary converters.
    enum FourCC {
        // 9 Primary YUV formats: 5 planar, 2 biplanar, 2 packed.
        FOURCC_I420 = CRICKET_FOURCC('I', '4', '2', '0'),
        FOURCC_I422 = CRICKET_FOURCC('I', '4', '2', '2'),
        FOURCC_I444 = CRICKET_FOURCC('I', '4', '4', '4'),
        FOURCC_I411 = CRICKET_FOURCC('I', '4', '1', '1'),
        FOURCC_I400 = CRICKET_FOURCC('I', '4', '0', '0'),
        FOURCC_NV21 = CRICKET_FOURCC('N', 'V', '2', '1'),
        FOURCC_NV12 = CRICKET_FOURCC('N', 'V', '1', '2'),
        FOURCC_YUY2 = CRICKET_FOURCC('Y', 'U', 'Y', '2'),
        FOURCC_UYVY = CRICKET_FOURCC('U', 'Y', 'V', 'Y'),

        // 2 Secondary YUV formats: row biplanar.
        FOURCC_M420 = CRICKET_FOURCC('M', '4', '2', '0'),

        // 9 Primary RGB formats: 4 32 bpp, 2 24 bpp, 3 16 bpp.
        FOURCC_ARGB = CRICKET_FOURCC('A', 'R', 'G', 'B'),
        FOURCC_BGRA = CRICKET_FOURCC('B', 'G', 'R', 'A'),
        FOURCC_ABGR = CRICKET_FOURCC('A', 'B', 'G', 'R'),
        FOURCC_24BG = CRICKET_FOURCC('2', '4', 'B', 'G'),
        FOURCC_RAW = CRICKET_FOURCC('r', 'a', 'w', ' '),
        FOURCC_RGBA = CRICKET_FOURCC('R', 'G', 'B', 'A'),
        FOURCC_RGBP = CRICKET_FOURCC('R', 'G', 'B', 'P'),  // bgr565.
        FOURCC_RGBO = CRICKET_FOURCC('R', 'G', 'B', 'O'),  // abgr1555.
        FOURCC_R444 = CRICKET_FOURCC('R', '4', '4', '4'),  // argb4444.

        // 4 Secondary RGB formats: 4 Bayer Patterns.
        FOURCC_RGGB = CRICKET_FOURCC('R', 'G', 'G', 'B'),
        FOURCC_BGGR = CRICKET_FOURCC('B', 'G', 'G', 'R'),
        FOURCC_GRBG = CRICKET_FOURCC('G', 'R', 'B', 'G'),
        FOURCC_GBRG = CRICKET_FOURCC('G', 'B', 'R', 'G'),

        // 1 Primary Compressed YUV format.
        FOURCC_MJPG = CRICKET_FOURCC('M', 'J', 'P', 'G'),

        // 5 Auxiliary YUV variations: 3 with U and V planes are swapped, 1 Alias.
        FOURCC_YV12 = CRICKET_FOURCC('Y', 'V', '1', '2'),
        FOURCC_YV16 = CRICKET_FOURCC('Y', 'V', '1', '6'),
        FOURCC_YV24 = CRICKET_FOURCC('Y', 'V', '2', '4'),
        FOURCC_YU12 = CRICKET_FOURCC('Y', 'U', '1', '2'),  // Linux version of I420.
        FOURCC_J420 = CRICKET_FOURCC('J', '4', '2', '0'),
        FOURCC_J400 = CRICKET_FOURCC('J', '4', '0', '0'),

        // 14 Auxiliary aliases.  CanonicalFourCC() maps these to canonical FOURCC.
        FOURCC_IYUV = CRICKET_FOURCC('I', 'Y', 'U', 'V'),  // Alias for I420.
        FOURCC_YU16 = CRICKET_FOURCC('Y', 'U', '1', '6'),  // Alias for I422.
        FOURCC_YU24 = CRICKET_FOURCC('Y', 'U', '2', '4'),  // Alias for I444.
        FOURCC_YUYV = CRICKET_FOURCC('Y', 'U', 'Y', 'V'),  // Alias for YUY2.
        FOURCC_YUVS = CRICKET_FOURCC('y', 'u', 'v', 's'),  // Alias for YUY2 on Mac.
        FOURCC_HDYC = CRICKET_FOURCC('H', 'D', 'Y', 'C'),  // Alias for UYVY.
        FOURCC_2VUY = CRICKET_FOURCC('2', 'v', 'u', 'y'),  // Alias for UYVY on Mac.
        FOURCC_JPEG = CRICKET_FOURCC('J', 'P', 'E', 'G'),  // Alias for MJPG.
        FOURCC_DMB1 = CRICKET_FOURCC('d', 'm', 'b', '1'),  // Alias for MJPG on Mac.
        FOURCC_BA81 = CRICKET_FOURCC('B', 'A', '8', '1'),  // Alias for BGGR.
        FOURCC_RGB3 = CRICKET_FOURCC('R', 'G', 'B', '3'),  // Alias for RAW.
        FOURCC_BGR3 = CRICKET_FOURCC('B', 'G', 'R', '3'),  // Alias for 24BG.
        FOURCC_CM32 = CRICKET_FOURCC(0, 0, 0, 32),  // BGRA kCMPixelFormat_32ARGB
        FOURCC_CM24 = CRICKET_FOURCC(0, 0, 0, 24),  // RAW kCMPixelFormat_24RGB

        // 1 Auxiliary compressed YUV format set aside for capturer.
        FOURCC_H264 = CRICKET_FOURCC('H', '2', '6', '4'),
    };

#undef CRICKET_FOURCC



// Returns the largest positive integer that divides both `a` and `b`.
    int GreatestCommonDivisor(int a, int b);

// Returns the smallest positive integer that is divisible by both `a` and `b`.
    int LeastCommonMultiple(int a, int b);
}//namespace kkrtc



#endif //KKRTC_VIDEO_COMMON_H
