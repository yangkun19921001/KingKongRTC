//
// Created by devyk on 2023/10/15.
//

#ifndef KKRTC_KKRTC_LIBYUV_H
#define KKRTC_KKRTC_LIBYUV_H

#include <stdint.h>
#include <stdio.h>

#include <vector>
#include "kkrtc_export.h"
#include "kkrtc_mediaformat.h"
/*
 * KKRTC's wrapper to libyuv.
 */

namespace kkrtc{

// This is the max PSNR value our algorithms can return.
    const double kPerfectPSNR = 48.0f;

    // Calculate the required buffer size.
    // Input:
    //   - type         :The type of the designated video frame.
    //   - width        :frame width in pixels.
    //   - height       :frame height in pixels.
    // Return value:    :The required size in bytes to accommodate the specified
    //                   video frame.
    size_t CalcBufferSize(KKVideoFormat type, int width, int height);
}//namespace


#endif //KKRTC_KKRTC_LIBYUV_H
