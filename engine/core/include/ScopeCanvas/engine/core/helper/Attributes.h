#pragma once

// SC_DEPRECATED(msg)
// Use before a declaration to mark it deprecated with a message.
// Example:
//   SC_DEPRECATED("Use Expr") struct ExprOld { ... };
#if defined(_MSC_VER)
#define SC_DEPRECATED(msg) __declspec(deprecated(msg))
#elif defined(__has_cpp_attribute)
#if __has_cpp_attribute(deprecated)
#define SC_DEPRECATED(msg) [[deprecated(msg)]]
#else
#if defined(__GNUC__) || defined(__clang__)
#define SC_DEPRECATED(msg) __attribute__((deprecated(msg)))
#else
#define SC_DEPRECATED(msg)
#endif
#endif
#elif defined(__GNUC__) || defined(__clang__)
#define SC_DEPRECATED(msg) __attribute__((deprecated(msg)))
#else
#define SC_DEPRECATED(msg)
#endif