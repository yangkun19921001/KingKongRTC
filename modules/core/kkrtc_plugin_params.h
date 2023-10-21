//
// Created by devyk on 2023/10/16.
//

#ifndef KKRTC_KKRTC_PLUGIN_PARAMS_H
#define KKRTC_KKRTC_PLUGIN_PARAMS_H
namespace kkrtc {

    typedef struct PluginGlobParam {
        void *glob_logger; //宿主 logger
        int *c_params;//参数
        unsigned n_params;//参数个数
    } PluginGlobParam_t;
}
#endif //KKRTC_KKRTC_PLUGIN_PARAMS_H
