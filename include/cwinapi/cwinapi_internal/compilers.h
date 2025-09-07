#ifndef WIN_INTERNAL_COMPILERS_H_
#define WIN_INTERNAL_COMPILERS_H_

#ifdef CWINAPI_BUILD_DLL
#  define CWINAPI_PUBLIC __declspec(dllexport)
#elif defined(CWINAPI_BUILD_STATIC) || defined(CWINAPI_USE_STATIC)
#  define CWINAPI_PUBLIC
#else
#  define CWINAPI_PUBLIC __declspec(dllimport)
#endif

#ifdef __GNUC__
#define CWINAPI_ALWAYS_INLINE __attribute__ ((always_inline))
#elif _MSC_VER
#define CWINAPI_ALWAYS_INLINE __forceinline
#else
#define CWINAPI_ALWAYS_INLINE
#endif


#else

#endif // INTERNAL_COMPILERS_H_
