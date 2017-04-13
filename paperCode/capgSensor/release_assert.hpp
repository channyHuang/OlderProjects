#ifndef BOOST_ASSERT_RELEASE_MODE_HACK_H__
#define BOOST_ASSERT_RELEASE_MODE_HACK_H__

/**
 * boost assert handle
 *
 * Usage : enable to handle assert in release mode
 */

#define BOOST_ENABLE_ASSERT_HANDLER
#include <boost/assert.hpp>

#ifndef BOOST_ASSERT_MSG_OSTREAM
#define BOOST_ASSERT_MSG_OSTREAM std::cerr
#endif

#include <cstdlib>
#include <iostream>
#include <boost/current_function.hpp>

namespace boost
{
inline void assertion_failed_msg(char const* expr, char const* msg,
                                 char const* function,
                                 char const* file, long line)
{
    BOOST_ASSERT_MSG_OSTREAM
            << "***** Internal Program Error - assertion (" << expr << ") failed in "
            << function << ":\n"
            << file << '(' << line << "): " << msg << std::endl;
    std::abort();
}

inline void assertion_failed(char const* expr, char const* function,
                             char const* file,
                             long line)
{
    BOOST_ASSERT_MSG_OSTREAM
            << "***** Internal Program Error - assertion (" << expr << ") failed in "
            << function << ":\n"
            << file << '(' << line << "): " << std::endl;
    std::abort();
}

} // boost

/**
 * if not disable boost assert and in release mode, then hack BOOST_ASSERT
 * and BOOST_ASSERT_MSG macro, makeing them still taking effect in release mode
 */
#if !defined(BOOST_DISABLE_ASSERTS) && defined(NDEBUG) && defined(WHILEDOING_RELEASE_BOOST_ASSERT)

#undef BOOST_ASSERT
#undef BOOST_ASSERT_MSG

#define BOOST_ASSERT(expr) ((expr) \
                            ? ((void)0) \
                            : ::boost::assertion_failed(#expr, BOOST_CURRENT_FUNCTION, __FILE__, __LINE__))

#define BOOST_ASSERT_MSG(expr, msg) ((expr) \
                                     ? ((void)0) \
                                     : ::boost::assertion_failed_msg(#expr, msg, BOOST_CURRENT_FUNCTION, __FILE__, __LINE__))

#endif

#endif /* end of include guard: BOOST_ASSERT_RELEASE_MODE_HACK_H__ */