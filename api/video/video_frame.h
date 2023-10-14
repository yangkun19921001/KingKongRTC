//
// Created by devyk on 2023/10/7.
//

#ifndef KKRTC_VIDEO_FRAME_H
#define KKRTC_VIDEO_FRAME_H


#include "kkrtc_export.h"
namespace kkrtc {
    class RTC_EXPORT VideoFrame {
    public:
        struct RTC_EXPORT UpdateRect {
            int offset_x;
            int offset_y;
            int width;
            int height;

            // Makes this UpdateRect a bounding box of this and other rect.
            void Union(const UpdateRect& other);

            // Makes this UpdateRect an intersection of this and other rect.
            void Intersect(const UpdateRect& other);

            // Sets everything to 0, making this UpdateRect a zero-size (empty) update.
            void MakeEmptyUpdate();

            bool IsEmpty() const;

            // Per-member equality check. Empty rectangles with different offsets would
            // be considered different.
            bool operator==(const UpdateRect& other) const {
                return other.offset_x == offset_x && other.offset_y == offset_y &&
                       other.width == width && other.height == height;
            }

            bool operator!=(const UpdateRect& other) const { return !(*this == other); }

            // Scales update_rect given original frame dimensions.
            // Cropping is applied first, then rect is scaled down.
            // Update rect is snapped to 2x2 grid due to possible UV subsampling and
            // then expanded by additional 2 pixels in each direction to accommodate any
            // possible scaling artifacts.
            // Note, close but not equal update_rects on original frame may result in
            // the same scaled update rects.
            UpdateRect ScaleWithFrame(int frame_width,
                                      int frame_height,
                                      int crop_x,
                                      int crop_y,
                                      int crop_width,
                                      int crop_height,
                                      int scaled_width,
                                      int scaled_height) const;
        };


    };

}//namespace KKRTC



#endif //KKRTC_VIDEO_FRAME_H
