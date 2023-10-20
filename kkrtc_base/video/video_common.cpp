//
// Created by devyk on 2023/10/20.
//

#include "video_common.h"

#include "array_view.h"
namespace cricket {

    int GreatestCommonDivisor(int a, int b) {
        int c = a % b;
        while (c != 0) {
            a = b;
            b = c;
            c = a % b;
        }
        return b;
    }

    int LeastCommonMultiple(int a, int b) {
        return a * (b / GreatestCommonDivisor(a, b));
    }
}//namespace kkrtc