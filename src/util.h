#ifndef SRC_UTIL_H_
#define SRC_UTIL_H_

#include "v8.h"

namespace xnode {

#define UNLIKELY(expr) expr

// #define CHECK(expr)                                                           \
//   do {                                                                        \
//     if (UNLIKELY(!(expr))) {                                                  \
//       ERROR_AND_ABORT(expr);                                                  \
//     }                                                                         \
//   } while (0)

#define CHECK_EQ(a, b) CHECK((a) == (b))
#define CHECK_GE(a, b) CHECK((a) >= (b))
#define CHECK_GT(a, b) CHECK((a) > (b))
#define CHECK_LE(a, b) CHECK((a) <= (b))
#define CHECK_LT(a, b) CHECK((a) < (b))
#define CHECK_NE(a, b) CHECK((a) != (b))
#define CHECK_NULL(val) CHECK((val) == nullptr)
#define CHECK_NOT_NULL(val) CHECK((val) != nullptr)
#define CHECK_IMPLIES(a, b) CHECK(!(a) || (b))

#ifdef DEBUG
  #define DCHECK(expr) CHECK(expr)
  #define DCHECK_EQ(a, b) CHECK((a) == (b))
  #define DCHECK_GE(a, b) CHECK((a) >= (b))
  #define DCHECK_GT(a, b) CHECK((a) > (b))
  #define DCHECK_LE(a, b) CHECK((a) <= (b))
  #define DCHECK_LT(a, b) CHECK((a) < (b))
  #define DCHECK_NE(a, b) CHECK((a) != (b))
  #define DCHECK_NULL(val) CHECK((val) == nullptr)
  #define DCHECK_NOT_NULL(val) CHECK((val) != nullptr)
  #define DCHECK_IMPLIES(a, b) CHECK(!(a) || (b))
#else
  #define DCHECK(expr)
  #define DCHECK_EQ(a, b)
  #define DCHECK_GE(a, b)
  #define DCHECK_GT(a, b)
  #define DCHECK_LE(a, b)
  #define DCHECK_LT(a, b)
  #define DCHECK_NE(a, b)
  #define DCHECK_NULL(val)
  #define DCHECK_NOT_NULL(val)
  #define DCHECK_IMPLIES(a, b)
#endif

}

#endif // SRC_UTIL_H_