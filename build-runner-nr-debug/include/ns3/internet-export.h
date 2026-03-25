
#ifndef INTERNET_EXPORT_H
#define INTERNET_EXPORT_H

#ifdef INTERNET_STATIC_DEFINE
#  define INTERNET_EXPORT
#  define INTERNET_NO_EXPORT
#else
#  ifndef INTERNET_EXPORT
#    ifdef internet_EXPORTS
        /* We are building this library */
#      define INTERNET_EXPORT __declspec(dllexport)
#    else
        /* We are using this library */
#      define INTERNET_EXPORT __declspec(dllimport)
#    endif
#  endif

#  ifndef INTERNET_NO_EXPORT
#    define INTERNET_NO_EXPORT 
#  endif
#endif

#ifndef INTERNET_DEPRECATED
#  define INTERNET_DEPRECATED __declspec(deprecated)
#endif

#ifndef INTERNET_DEPRECATED_EXPORT
#  define INTERNET_DEPRECATED_EXPORT INTERNET_EXPORT INTERNET_DEPRECATED
#endif

#ifndef INTERNET_DEPRECATED_NO_EXPORT
#  define INTERNET_DEPRECATED_NO_EXPORT INTERNET_NO_EXPORT INTERNET_DEPRECATED
#endif

/* NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if) */
#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef INTERNET_NO_DEPRECATED
#    define INTERNET_NO_DEPRECATED
#  endif
#endif

// Undefine the *_EXPORT symbols for non-Windows based builds
#ifndef NS_MSVC
#undef INTERNET_EXPORT
#define INTERNET_EXPORT
#undef INTERNET_NO_EXPORT
#define INTERNET_NO_EXPORT
#endif
#endif /* INTERNET_EXPORT_H */
