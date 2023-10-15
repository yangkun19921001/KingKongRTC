//
// Created by devyk on 2023/9/15.
//

#ifndef KKRTC_KKRTC_MEDIAFORMAT_H
#define KKRTC_KKRTC_MEDIAFORMAT_H

#include <vector>
#include <algorithm>
#include "log_helper.h"

namespace kkrtc {

    enum KKVideoFormat {
        Unknown = -1,
        I420,
        IYUV,
        RGB24,
        ARGB,
        RGB565,
        YUY2,
        YV12,
        UYVY,
        MJPEG,
        BGRA,
        YVYU,
        NV12,
        NV21
    };

    template <class T>
    inline T castParameterTo(int paramValue)
    {
        return static_cast<T>(paramValue);
    }

    template <>
    inline bool castParameterTo(int paramValue)
    {
        return paramValue != 0;
    }

    class KKMediaFormat
    {
    public:
        struct MediaFormat {
            MediaFormat() = default;

            MediaFormat(int key_, int value_) : key(key_), value(value_) {}

            int key{-1};
            int value{-1};
            mutable bool isConsumed{false};
        };

        KKMediaFormat() = default;

        explicit KKMediaFormat(const std::vector<int>& params)
        {
            const auto count = params.size();
            if (count % 2 != 0)
            {
                /*CV_Error_(Error::StsVecLengthErr,
                          ("Vector of VideoWriter parameters should have even length"));*/
            }
            params_.reserve(count / 2);
            for (std::size_t i = 0; i < count; i += 2)
            {
                add(params[i], params[i + 1]);
            }
        }

        KKMediaFormat(int* params, unsigned n_params)
        {
            params_.reserve(n_params);
            for (unsigned i = 0; i < n_params; ++i)
            {
                add(params[2*i], params[2*i + 1]);
            }
        }

        void add(int key, int value)
        {
            params_.emplace_back(key, value);
        }

        bool has(int key) const
        {
            auto it = std::find_if(params_.begin(), params_.end(),
                                   [key](const MediaFormat &param)
                                   {
                                       return param.key == key;
                                   }
            );
            return it != params_.end();
        }

        template <class ValueType>
        ValueType get(int key) const
        {
            auto it = std::find_if(params_.begin(), params_.end(),
                                   [key](const MediaFormat &param)
                                   {
                                       return param.key == key;
                                   }
            );
            if (it != params_.end())
            {
                it->isConsumed = true;
                return castParameterTo<ValueType>(it->value);
            }
            else
            {
                KKLogErrorTag("KKMediaFormat")<<"Missing value for parameter: "<<key;
            }
        }

        template <class ValueType>
        ValueType get(int key, ValueType defaultValue) const
        {
            auto it = std::find_if(params_.begin(), params_.end(),
                                   [key](const MediaFormat &param)
                                   {
                                       return param.key == key;
                                   }
            );
            if (it != params_.end())
            {
                it->isConsumed = true;
                return castParameterTo<ValueType>(it->value);
            }
            else
            {
                return defaultValue;
            }
        }

        std::vector<int> getUnused() const
        {
            std::vector<int> unusedParams;
            for (const auto &param : params_)
            {
                if (!param.isConsumed)
                {
                    unusedParams.push_back(param.key);
                }
            }
            return unusedParams;
        }

        std::vector<int> getIntVector() const
        {
            std::vector<int> vint_params;
            for (const auto& param : params_)
            {
                vint_params.push_back(param.key);
                vint_params.push_back(param.value);
            }
            return vint_params;
        }

        bool empty() const
        {
            return params_.empty();
        }

        bool warnUnusedParameters() const
        {
            bool found = false;
            for (const auto &param : params_)
            {
                if (!param.isConsumed)
                {
                    found = true;
                    KKLogInfoTag("KKMediaFormat")<< "VIDEOIO: unused parameter: [" << param.key << "]=" <<
                                                                     std::format("%lld / 0x%016llx", (long long)param.value, (long long)param.value);
                }
            }
            return found;
        }


    private:
        std::vector<MediaFormat> params_;
    };
}
#endif //KKRTC_KKRTC_MEDIAFORMAT_H
