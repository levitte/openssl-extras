Nuggets for use with OpenSSL
============================

-   btrace.c, btrace.h

    A simple tracing filter, useful if you lose track of where a
    particular BIO is used and you want to figure out its ins and
    outs.  For deeper inspection, launch your favorit debugger and
    set breakpoints on the internal functions.

-   cmake/Modules/FindOpenSSL.cmake

    A cmake hack on top of the regular FindOpenSSL.cmake, which allows
    setting the OpenSSL root directory to a build directory, and that
    also generates OPENSSL_ROOT_DIR if it's not already defined
    (because the user chose to search along CMAKE_PREFIX_PATH).

    It's self-documented.

-   perl/lib/WrapOpenSSL.pm

    A plugin for App::Prove, the harness application ('prove') to be
    used to run Test::More, Test2::Suite and similar test suites.  It
    works best in conjunction with the cmake module above.

