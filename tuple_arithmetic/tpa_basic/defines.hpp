#pragma once

#ifndef TP_NO_NAMESPACE
# ifndef TP_NAMESPACE
#  define TP_NAMESPACE tpa
# endif
#endif

#ifndef FORCE_INLINE
#  if defined(_MSC_VER)
#    define FORCE_INLINE __forceinline
#  else
#    define FORCE_INLINE inline __attribute__((always_inline))
#    ifndef __GNUC__
#        pragma STDC FP_CONTRACT ON
#    endif
#  endif
#endif

#if defined(TP_NAMESPACE)
# define TP_ENTER_NS namespace TP_NAMESPACE {
# define TP_EXIT_NS }
# define TP_IN_NS(name) TP_NAMESPACE::name
#else
# define TP_ENTER_NS
# define TP_EXIT_NS
# define TP_IN_NS(name) name
#endif

#if not defined(TP_DONOT_CONVERT)
#define TP_CONVERT(tp) to_array(tp)
#else
#define TP_CONVERT(tp) (tp)
#endif
