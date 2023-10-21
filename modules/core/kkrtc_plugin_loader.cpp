//
// Created by devyk on 2023/9/17.
//

#include "kkrtc_plugin_loader.h"
#include "kkrtc_std.h"

#if !KKRTC_HAVE_FILESYSTEM_SUPPORT
#error "Invalid build configuration"
#endif

namespace kkrtc {
    namespace plugin {
        std::vector<plugin::FileSystemPath_t> getPluginCandidates(const std::string &baseName) {
            using namespace kkrtc::utils;
            using namespace kkrtc::utils::fs;
            using namespace plugin;
            const std::string baseName_l = kkrtc::toLowerCase(baseName);
            const std::string baseName_u = kkrtc::toUpperCase(baseName);
            const plugin::FileSystemPath_t baseName_l_fs = plugin::toFileSystemPath(baseName_l);
            std::vector<plugin::FileSystemPath_t> paths;
            //todo
            const std::vector<std::string> paths_ /*= getConfigurationParameterPaths("KKRTC_VIDEO_CAP_PLUGIN_PATH", std::vector<std::string>())*/;
            if (paths_.size() != 0) {
                for (size_t i = 0; i < paths_.size(); i++) {
                    paths.push_back(plugin::toFileSystemPath(paths_[i]));
                }
            } else {
                FileSystemPath_t binaryLocation;
                //todo
                if (getBinLocation(binaryLocation)) {
                    binaryLocation = getParent(binaryLocation);
#ifndef KK_VIDEO_CAP_PLUGIN_SUBDIRECTORY
                    paths.push_back(binaryLocation);
#else
#ifdef KKRTC_WIN
                    paths.push_back(binaryLocation + toFileSystemPath("\\") + toFileSystemPath("plugins"));
#else
                    paths.push_back(binaryLocation + toFileSystemPath("/") + toFileSystemPath("plugins"));

#endif
#endif

                }
            }
            const std::string default_expr =
                    plugin::libraryPrefix() + "kkrtc_" + baseName_l /*+ "*"*/ + plugin::librarySuffix();
            //const std::string default_expr = "win_capture_ds_lib.dll";
            //todo
            // const std::string plugin_expr /*= getConfigurationParameterString((std::string("KKRTC_VIDEO_CAP_PLUGIN_") + baseName_u).c_str(), default_expr.c_str())*/;
            const std::string plugin_expr = default_expr;
            std::vector<plugin::FileSystemPath_t> results;
#ifdef _WIN32
            plugin::FileSystemPath_t moduleName = plugin::toFileSystemPath(
                    plugin::libraryPrefix() + "kkrtc_" + baseName_l + plugin::librarySuffix());
            //plugin::FileSystemPath_t moduleName  = plugin::toFileSystemPath("win_capture_ds_lib.dll");

#ifndef WINRT
            if (baseName_u == "FFMPEG")  // backward compatibility
            {
                const wchar_t *ffmpeg_env_path = _wgetenv(L"KKRTC_FFMPEG_DLL_DIR");
                if (ffmpeg_env_path) {
                    results.push_back(plugin::FileSystemPath_t(ffmpeg_env_path) + L"\\" + moduleName);
                }
            }
#endif
            if (plugin_expr != default_expr) {
                moduleName = plugin::toFileSystemPath(plugin_expr);
                results.push_back(moduleName);
            }
            for (const plugin::FileSystemPath_t &path: paths) {
                results.push_back(path + L"\\" + moduleName);
            }

#if defined(_DEBUG) && defined(DEBUG_POSTFIX)
            if (baseName_u == "FFMPEG")  // backward compatibility
    {
        const FileSystemPath_t templ = toFileSystemPath(KKAUX_STR(DEBUG_POSTFIX) ".dll");
        FileSystemPath_t nonDebugName(moduleName);
        size_t suf = nonDebugName.rfind(templ);
        if (suf != FileSystemPath_t::npos)
        {
            nonDebugName.replace(suf, suf + templ.size(), L".dll");
            results.push_back(nonDebugName);
        }
    }
#endif // _DEBUG && DEBUG_POSTFIX
#else
            KKLogInfoTag("kkrtc_cap_backend_plugin")<< "VideoIO plugin (" << baseName << "): glob is '" << plugin_expr << "', " << paths.size() << " location(s)";
          for (const std::string& path : paths)
          {
              if (path.empty())
                  continue;
              std::vector<std::string> candidates;
              cv::glob(utils::fs::join(path, plugin_expr), candidates);
              // Prefer candisates with higher versions
              // TODO: implemented accurate versions-based comparator
              std::sort(candidates.begin(), candidates.end(), std::greater<std::string>());
              KKLogInfoTag("kkrtc_cap_backend_plugin")<< "    - " << path << ": " << candidates.size());
              copy(candidates.begin(), candidates.end(), back_inserter(results));
          }
#endif
            KKLogInfoTag("kkrtc_backend_plugin") << "Found " << results.size() << " plugin(s) for " << baseName;
            return results;
        }


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