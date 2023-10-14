//
// Created by devyk on 2023/10/7.
//

#ifndef KKRTC_KKRTC_EXPORT_H
#define KKRTC_KKRTC_EXPORT_H


// RTC_EXPORT is used to mark symbols as exported or imported when KKRTC is
// built or used as a shared library.
// When KKRTC is built as a static library the RTC_EXPORT macro expands to
// nothing.

#ifdef KKRTC_ENABLE_SYMBOL_EXPORT

#ifdef KKRTC_WIN

#ifdef KKRTC_LIBRARY_IMPL
#define RTC_EXPORT __declspec(dllexport)
#else
#define RTC_EXPORT __declspec(dllimport)
#endif

#else  // KKRTC_WIN

#if __has_attribute(visibility) && defined(KKRTC_LIBRARY_IMPL)
#define RTC_EXPORT __attribute__((visibility("default")))
#endif

#endif  // KKRTC_WIN

#endif  // KKRTC_ENABLE_SYMBOL_EXPORT

#ifndef RTC_EXPORT
#define RTC_EXPORT
#endif


#endif //KKRTC_KKRTC_EXPORT_H
