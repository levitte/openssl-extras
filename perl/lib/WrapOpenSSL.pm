#! /usr/bin/env perl
#
# This is free and unencumbered software released into the public domain.
# For more information, please refer to <http://unlicense.org/>
# Original author: Richard Levitte <richard@levitte.org>

# This is a plugin for App::Prove, with the purpose of ensuring that the
# necessary environment to run programs that are linked with OpenSSL.
#
# It's designed to work with cmake, so it depends on the presence of at
# least the environment variable OPENSSL_ROOT_DIR.  However, if the
# environment variables OPENSSL_CRYPTO_LIBRARY and OPENSSL_PROGRAM are
# present too, it puts them to good use.
#
# Furthermore, it looks at CTEST_INTERACTIVE_DEBUG_MODE to determine if
# the run should be verbose or not.
#
# Environment affected:
#
# HARNESS_VERBOSE       Assigned 1 for verbose mode
# LD_LIBRARY_PATH       Linux, ELF, HP-UX shared library load path
# DYLD_LIBRARY_PATH     MacOS X shared library load path
# LIBPATH               AIX, OS/2 shared library load path
# PATH                  Universal program path
#
# Usage is as follows (PERL5LIB is used because some 'prove' versions seem
# to ignore '-I'):
#
# PERL5LIB=/path/to/this/script prove -PWrapOpenSSL {args...}

package App::Prove::Plugin::WrapOpenSSL;
use strict;
use warnings;

use File::Basename;
use File::Spec::Functions;

sub load {
    my ($class, $p) = @_;
    my $app  = $p->{app_prove};

    # turn on verbosity
    my $verbose = $ENV{CTEST_INTERACTIVE_DEBUG_MODE} || $app->verbose();
    $app->verbose( $verbose );
    $ENV{HARNESS_VERBOSE} = 1 if $verbose;

    print STDERR "$_=", $ENV{$_} // '', "\n"
        foreach qw(OPENSSL_CRYPTO_LIBRARY OPENSSL_PROGRAM OPENSSL_ROOT_DIR);

    my $openssl_libdir = dirname($ENV{OPENSSL_CRYPTO_LIBRARY})
        if $ENV{OPENSSL_CRYPTO_LIBRARY};
    my $openssl_bindir = dirname($ENV{OPENSSL_PROGRAM})
        if $ENV{OPENSSL_PROGRAM};
    my $openssl_rootdir = $ENV{OPENSSL_ROOT_DIR};
    my $openssl_rootdir_is_buildtree =
        $openssl_rootdir && -d catdir($openssl_rootdir, 'configdata.pm');

    unless ($openssl_libdir) {
        $openssl_libdir = $openssl_rootdir_is_buildtree
            ? $openssl_rootdir
            : catdir($openssl_rootdir, 'lib');
    }
    unless ($openssl_bindir) {
        $openssl_bindir = $openssl_rootdir_is_buildtree
            ? catdir($openssl_rootdir, 'apps')
            : catdir($openssl_rootdir, 'bin');
    }

    if ($openssl_libdir) {
        # Variants of library paths
        $ENV{$_} = join(':', $openssl_libdir, $ENV{$_} // ())
            foreach (
                     'LD_LIBRARY_PATH',    # Linux, ELF HP-UX
                     'DYLD_LIBRARY_PATH',  # MacOS X
                     'LIBPATH',            # AIX, OS/2
            );
        if ($verbose) {
            print STDERR "Added $openssl_libdir to:\n";
            print STDERR "  LD_LIBRARY_PATH, DYLD_LIBRARY_PATH, LIBPATH\n";
        }
    }

    if ($openssl_bindir) {
        # Binary path, works the same everywhere
        $ENV{PATH} = join(':', $openssl_bindir, $ENV{PATH});
        if ($verbose) {
            print STDERR "Added $openssl_bindir to:\n";
            print STDERR "  PATH\n";
        }
    }
    if ($verbose) {
        print STDERR "$_=", $ENV{$_} // '', "\n"
            foreach qw(LD_LIBRARY_PATH DYLD_LIBRARY_PATH LIBPATH PATH);
    }
}

1;
