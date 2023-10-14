//
// Created by devyk on 2023/9/17.
//

#include "kkrtc_plugin_loader.h"

#if !KKRTC_HAVE_FILESYSTEM_SUPPORT
#error "Invalid build configuration"
#endif

namespace kkrtc {
    namespace plugin {

        DynamicLib::DynamicLib(const FileSystemPath_t& filename)
                : handle(0), fname(filename), disableAutoUnloading_(false)
        {
            libraryLoad(filename);
        }

        DynamicLib::~DynamicLib()
        {
            if (!disableAutoUnloading_)
            {
                libraryRelease();
            }
            else if (handle)
            {
                KKLogInfoTag("DynamicLib") << "skip auto unloading (disabled): " << toPrintablePath(fname);
                handle = 0;
            }
        }

        void* DynamicLib::getSymbol(const char* symbolName) const
        {
            if (!handle)
            {
                return 0;
            }
            void* res = getSymbol_(handle, symbolName);
            if (!res)
            {
                KKLogDebugTag("DynamicLib") <<  "No symbol '" << symbolName << "' in " << toPrintablePath(fname);

            }
            return res;
        }

        std::string DynamicLib::getName() const
        {
            return toPrintablePath(fname);
        }

        void DynamicLib::libraryLoad(const FileSystemPath_t& filename)
        {
            handle = libraryLoad_(filename);
            KKLogInfoTag("DynamicLib") << "load " << toPrintablePath(filename) << " => " << (handle ? "OK" : "FAILED");
        }

        void DynamicLib::libraryRelease()
        {
            if (handle)
            {
                KKLogInfoTag("DynamicLib") <<"unload "<< toPrintablePath(fname);
                libraryRelease_(handle);
                handle = 0;
            }
        }
    }
}