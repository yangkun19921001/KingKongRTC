//
// Created by devyk on 2023/9/19.
//

#ifndef KKRTC_KKRTC_STD_WRAPPER_H
#define KKRTC_KKRTC_STD_WRAPPER_H

#include <string>
#include <memory>  // std::shared_ptr
#include <type_traits>  // std::enable_if

namespace kkrtc {

    using std::nullptr_t;

//! @addtogroup core_basic
//! @{

#ifdef CV_DOXYGEN

    template <typename _Tp> using Ptr = std::shared_ptr<_Tp>;  // In ideal world it should look like this, but we need some compatibility workarounds below

template<typename _Tp, typename ... A1> static inline
Ptr<_Tp> makePtr(const A1&... a1) { return std::make_shared<_Tp>(a1...); }

#else  // cv::Ptr with compatibility workarounds

// It should be defined for C-API types only.
// C++ types should use regular "delete" operator.
    template<typename Y> struct DefaultDeleter;
#if 0
    {
    void operator()(Y* p) const;
};
#endif

    namespace sfinae {
        template<typename C, typename Ret, typename... Args>
        struct has_parenthesis_operator
        {
        private:
            template<typename T>
            static  std::true_type has_parenthesis_operator_check(typename std::is_same<typename std::decay<decltype(std::declval<T>().operator()(std::declval<Args>()...))>::type, Ret>::type*);

            template<typename> static  std::false_type has_parenthesis_operator_check(...);

            typedef decltype(has_parenthesis_operator_check<C>(0)) type;

        public:
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1900/*MSVS 2015*/)
            static constexpr bool value = type::value;
#else
            // support MSVS 2013
    static const int value = type::value;
#endif
        };
    } // namespace sfinae

    template <typename T, typename = void>
    struct has_custom_delete
            : public std::false_type {};

// Force has_custom_delete to std::false_type when NVCC is compiling CUDA source files
#ifndef __CUDACC__
    template <typename T>
    struct has_custom_delete<T, typename std::enable_if< sfinae::has_parenthesis_operator<DefaultDeleter<T>, void, T*>::value >::type >
            : public std::true_type {};
#endif

    template<typename T>
    struct KKPtr : public std::shared_ptr<T>
    {
#if 0
        using std::shared_ptr<T>::shared_ptr;  // GCC 5.x can't handle this
#else
        inline KKPtr()  : std::shared_ptr<T>() {}
        inline KKPtr(nullptr_t)  : std::shared_ptr<T>(nullptr) {}
        template<typename Y, typename D> inline KKPtr(Y* p, D d) : std::shared_ptr<T>(p, d) {}
        template<typename D> inline KKPtr(nullptr_t, D d) : std::shared_ptr<T>(nullptr, d) {}

        template<typename Y> inline KKPtr(const KKPtr<Y>& r, T* ptr)  : std::shared_ptr<T>(r, ptr) {}

        inline KKPtr(const KKPtr<T>& o)  : std::shared_ptr<T>(o) {}
        inline KKPtr(KKPtr<T>&& o)  : std::shared_ptr<T>(std::move(o)) {}

        template<typename Y> inline KKPtr(const KKPtr<Y>& o)  : std::shared_ptr<T>(o) {}
        template<typename Y> inline KKPtr(KKPtr<Y>&& o)  : std::shared_ptr<T>(std::move(o)) {}
#endif
        inline KKPtr(const std::shared_ptr<T>& o)  : std::shared_ptr<T>(o) {}
        inline KKPtr(std::shared_ptr<T>&& o)  : std::shared_ptr<T>(std::move(o)) {}

        // Overload with custom DefaultDeleter: Ptr<IplImage>(...)
        template<typename Y>
        inline KKPtr(const std::true_type&, Y* ptr) : std::shared_ptr<T>(ptr, DefaultDeleter<Y>()) {}

        // Overload without custom deleter: Ptr<std::string>(...);
        template<typename Y>
        inline KKPtr(const std::false_type&, Y* ptr) : std::shared_ptr<T>(ptr) {}

        template<typename Y = T>
        inline KKPtr(Y* ptr) : KKPtr(has_custom_delete<Y>(), ptr) {}

        // Overload with custom DefaultDeleter: Ptr<IplImage>(...)
        template<typename Y>
        inline void reset(const std::true_type&, Y* ptr) { std::shared_ptr<T>::reset(ptr, DefaultDeleter<Y>()); }

        // Overload without custom deleter: Ptr<std::string>(...);
        template<typename Y>
        inline void reset(const std::false_type&, Y* ptr) { std::shared_ptr<T>::reset(ptr); }

        template<typename Y>
        inline void reset(Y* ptr) { KKPtr<T>::reset(has_custom_delete<Y>(), ptr); }

        template<class Y, class Deleter>
        void reset(Y* ptr, Deleter d) { std::shared_ptr<T>::reset(ptr, d); }

        void reset()  { std::shared_ptr<T>::reset(); }

        KKPtr& operator=(const KKPtr& o) { std::shared_ptr<T>::operator =(o); return *this; }
        template<typename Y> inline KKPtr& operator=(const KKPtr<Y>& o) { std::shared_ptr<T>::operator =(o); return *this; }

        T* operator->() const  { return std::shared_ptr<T>::get();}
        typename std::add_lvalue_reference<T>::type operator*() const  { return *std::shared_ptr<T>::get(); }

        // OpenCV 3.x methods (not a part of standard C++ library)
        inline void release() { std::shared_ptr<T>::reset(); }
        inline operator T* () const { return std::shared_ptr<T>::get(); }
        inline bool empty() const { return std::shared_ptr<T>::get() == nullptr; }

        template<typename Y> inline
        KKPtr<Y> staticCast() const  { return std::static_pointer_cast<Y>(*this); }

        template<typename Y> inline
        KKPtr<Y> constCast() const  { return std::const_pointer_cast<Y>(*this); }

        template<typename Y> inline
        KKPtr<Y> dynamicCast() const  { return std::dynamic_pointer_cast<Y>(*this); }
    };

    template<typename _Tp, typename ... A1> static inline
    KKPtr<_Tp> makePtr(const A1&... a1)
    {
        static_assert( !has_custom_delete<_Tp>::value, "Can't use this makePtr with custom DefaultDeleter");
        return (KKPtr<_Tp>)std::make_shared<_Tp>(a1...);
    }

#endif

//! @} core_basic
} // kkrtc

#endif //KKRTC_KKRTC_STD_WRAPPER_H
