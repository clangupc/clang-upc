dnl **************************************************************************
dnl * Initialize
dnl **************************************************************************
AC_INIT([[[SAMPLE]]],[[[x.xx]]],[bugs@yourdomain])

dnl Identify where LLVM source tree is
LLVM_SRC_ROOT="../../../.."
LLVM_OBJ_ROOT="../../../.."

dnl Find absolute paths to LLVM source and object trees
LLVM_ABS_SRC_ROOT="`cd $srcdir ; cd $LLVM_SRC_ROOT ; pwd`"
LLVM_ABS_OBJ_ROOT="`cd $LLVM_OBJ_ROOT ; pwd`"

dnl Tell autoconf that this is an LLVM project being configured
dnl This provides the --with-llvmsrc and --with-llvmobj options
LLVM_CONFIG_PROJECT($LLVM_ABS_SRC_ROOT,$LLVM_ABS_OBJ_ROOT)

dnl Try and find an llvm-config in the build directory. We are only using this
dnl to detect the package level LLVM information (currently just the version),
dnl so we just whatever one we find regardless of build mode.
AC_MSG_CHECKING([llvm-config])
llvm_config_path="`ls -1 $llvm_obj/*/bin/llvm-config 2> /dev/null | head -1`"
if ! test -f "$llvm_config_path" ; then
   llvm_config_path="no"
fi
AC_MSG_RESULT([$llvm_config_path])

dnl Determine the LLVM version, which may be required by the current Makefile
dnl rules.
AC_MSG_CHECKING([LLVM package version])
if test "$llvm_config_path" != no ; then
  llvm_package_version=`$llvm_config_path --version`
else
  llvm_package_version="unknown";
fi
AC_MSG_RESULT([$llvm_package_version])
AC_SUBST(LLVM_VERSION, [$llvm_package_version])

dnl Place all of the extra autoconf files into the config subdirectory. Tell
dnl various tools where the m4 autoconf macros are.
AC_CONFIG_AUX_DIR([autoconf])

dnl **************************************************************************
dnl Begin LLVM configure.ac Import
dnl **************************************************************************
dnl
dnl Derived from LLVM's configure.ac. This was imported directly here so that we
dnl could reuse LLVM's build infrastructure without introducing a direct source
dnl dependency on the LLVM files.

dnl We need to check for the compiler up here to avoid anything else
dnl starting with a different one.
AC_PROG_CC(clang llvm-gcc gcc)
AC_PROG_CXX(clang++ llvm-g++ g++)
AC_PROG_CPP

dnl Configure all of the projects present in our source tree. While we could
dnl just AC_CONFIG_SUBDIRS on the set of directories in projects that have a
dnl configure script, that usage of the AC_CONFIG_SUBDIRS macro is deprecated.
dnl Instead we match on the known projects.

dnl
dnl One tricky part of doing this is that some projects depend upon other
dnl projects.  For example, several projects rely upon the LLVM test suite.
dnl We want to configure those projects first so that their object trees are
dnl created before running the configure scripts of projects that depend upon
dnl them.
dnl

dnl Disable the build of polly, even if it is checked out into tools/polly.
AC_ARG_ENABLE(polly,
              AS_HELP_STRING([--enable-polly],
                             [Use polly if available (default is YES)]),,
                             enableval=default)
case "$enableval" in
  yes) AC_SUBST(ENABLE_POLLY,[1]) ;;
  no)  AC_SUBST(ENABLE_POLLY,[0]) ;;
  default) AC_SUBST(ENABLE_POLLY,[1]) ;;
  *) AC_MSG_ERROR([Invalid setting for --enable-polly. Use "yes" or "no"]) ;;
esac


dnl Check if polly is checked out into tools/polly and configure it if
dnl available.
if (test -d ${srcdir}/tools/polly) && (test $ENABLE_POLLY -eq 1) ; then
  AC_SUBST(LLVM_HAS_POLLY,1)
  AC_CONFIG_SUBDIRS([tools/polly])
fi

dnl===-----------------------------------------------------------------------===
dnl===
dnl=== SECTION 2: Architecture, target, and host checks
dnl===
dnl===-----------------------------------------------------------------------===

dnl Check the target for which we're compiling and the host that will do the
dnl compilations. This will tell us which LLVM compiler will be used for
dnl compiling SSA into object code. This needs to be done early because
dnl following tests depend on it.
AC_CANONICAL_TARGET

dnl Determine the platform type and cache its value. This helps us configure
dnl the System library to the correct build platform.
AC_CACHE_CHECK([type of operating system we're going to host on],
               [llvm_cv_os_type],
[case $host in
  *-*-aix*)
    llvm_cv_link_all_option="-Wl,--whole-archive"
    llvm_cv_no_link_all_option="-Wl,--no-whole-archive"
    llvm_cv_os_type="AIX"
    llvm_cv_platform_type="Unix" ;;
  *-*-irix*)
    llvm_cv_link_all_option="-Wl,--whole-archive"
    llvm_cv_no_link_all_option="-Wl,--no-whole-archive"
    llvm_cv_os_type="IRIX"
    llvm_cv_platform_type="Unix" ;;
  *-*-cygwin*)
    llvm_cv_link_all_option="-Wl,--whole-archive"
    llvm_cv_no_link_all_option="-Wl,--no-whole-archive"
    llvm_cv_os_type="Cygwin"
    llvm_cv_platform_type="Unix" ;;
  *-*-darwin*)
    llvm_cv_link_all_option="-Wl,-all_load"
    llvm_cv_no_link_all_option="-Wl,-noall_load"
    llvm_cv_os_type="Darwin"
    llvm_cv_platform_type="Unix" ;;
  *-*-minix*)
    llvm_cv_link_all_option="-Wl,-all_load"
    llvm_cv_no_link_all_option="-Wl,-noall_load"
    llvm_cv_os_type="Minix"
    llvm_cv_platform_type="Unix" ;;
  *-*-freebsd* | *-*-kfreebsd-gnu)
    llvm_cv_link_all_option="-Wl,--whole-archive"
    llvm_cv_no_link_all_option="-Wl,--no-whole-archive"
    llvm_cv_os_type="FreeBSD"
    llvm_cv_platform_type="Unix" ;;
  *-*-openbsd*)
    llvm_cv_link_all_option="-Wl,--whole-archive"
    llvm_cv_no_link_all_option="-Wl,--no-whole-archive"
    llvm_cv_os_type="OpenBSD"
    llvm_cv_platform_type="Unix" ;;
  *-*-netbsd*)
    llvm_cv_link_all_option="-Wl,--whole-archive"
    llvm_cv_no_link_all_option="-Wl,--no-whole-archive"
    llvm_cv_os_type="NetBSD"
    llvm_cv_platform_type="Unix" ;;
  *-*-dragonfly*)
    llvm_cv_link_all_option="-Wl,--whole-archive"
    llvm_cv_no_link_all_option="-Wl,--no-whole-archive"
    llvm_cv_os_type="DragonFly"
    llvm_cv_platform_type="Unix" ;;
  *-*-hpux*)
    llvm_cv_link_all_option="-Wl,--whole-archive"
    llvm_cv_no_link_all_option="-Wl,--no-whole-archive"
    llvm_cv_os_type="HP-UX"
    llvm_cv_platform_type="Unix" ;;
  *-*-interix*)
    llvm_cv_link_all_option="-Wl,--whole-archive"
    llvm_cv_no_link_all_option="-Wl,--no-whole-archive"
    llvm_cv_os_type="Interix"
    llvm_cv_platform_type="Unix" ;;
  *-*-linux*)
    llvm_cv_link_all_option="-Wl,--whole-archive"
    llvm_cv_no_link_all_option="-Wl,--no-whole-archive"
    llvm_cv_os_type="Linux"
    llvm_cv_platform_type="Unix" ;;
  *-*-gnu*)
    llvm_cv_link_all_option="-Wl,--whole-archive"
    llvm_cv_no_link_all_option="-Wl,--no-whole-archive"
    llvm_cv_os_type="GNU"
    llvm_cv_platform_type="Unix" ;;
  *-*-solaris*)
    llvm_cv_link_all_option="-Wl,-z,allextract"
    llvm_cv_no_link_all_option="-Wl,-z,defaultextract"
    llvm_cv_os_type="SunOS"
    llvm_cv_platform_type="Unix" ;;
  *-*-auroraux*)
    llvm_cv_link_all_option="-Wl,-z,allextract"
    llvm_cv_link_all_option="-Wl,-z,defaultextract"
    llvm_cv_os_type="AuroraUX"
    llvm_cv_platform_type="Unix" ;;
  *-*-win32*)
    llvm_cv_link_all_option="-Wl,--whole-archive"
    llvm_cv_no_link_all_option="-Wl,--no-whole-archive"
    llvm_cv_os_type="Win32"
    llvm_cv_platform_type="Win32" ;;
  *-*-mingw*)
    llvm_cv_link_all_option="-Wl,--whole-archive"
    llvm_cv_no_link_all_option="-Wl,--no-whole-archive"
    llvm_cv_os_type="MingW"
    llvm_cv_platform_type="Win32" ;;
  *-*-haiku*)
    llvm_cv_link_all_option="-Wl,--whole-archive"
    llvm_cv_no_link_all_option="-Wl,--no-whole-archive"
    llvm_cv_os_type="Haiku"
    llvm_cv_platform_type="Unix" ;;
  *-unknown-eabi*)
    llvm_cv_link_all_option="-Wl,--whole-archive"
    llvm_cv_no_link_all_option="-Wl,--no-whole-archive"
    llvm_cv_os_type="Freestanding"
    llvm_cv_platform_type="Unix" ;;
  *-unknown-elf*)
    llvm_cv_link_all_option="-Wl,--whole-archive"
    llvm_cv_no_link_all_option="-Wl,--no-whole-archive"
    llvm_cv_os_type="Freestanding"
    llvm_cv_platform_type="Unix" ;;
  *)
    llvm_cv_link_all_option=""
    llvm_cv_no_link_all_option=""
    llvm_cv_os_type="Unknown"
    llvm_cv_platform_type="Unknown" ;;
esac])

AC_CACHE_CHECK([type of operating system we're going to target],
               [llvm_cv_target_os_type],
[case $target in
  *-*-aix*)
    llvm_cv_target_os_type="AIX" ;;
  *-*-irix*)
    llvm_cv_target_os_type="IRIX" ;;
  *-*-cygwin*)
    llvm_cv_target_os_type="Cygwin" ;;
  *-*-darwin*)
    llvm_cv_target_os_type="Darwin" ;;
  *-*-minix*)
    llvm_cv_target_os_type="Minix" ;;
  *-*-freebsd* | *-*-kfreebsd-gnu)
    llvm_cv_target_os_type="FreeBSD" ;;
  *-*-openbsd*)
    llvm_cv_target_os_type="OpenBSD" ;;
  *-*-netbsd*)
    llvm_cv_target_os_type="NetBSD" ;;
  *-*-dragonfly*)
    llvm_cv_target_os_type="DragonFly" ;;
  *-*-hpux*)
    llvm_cv_target_os_type="HP-UX" ;;
  *-*-interix*)
    llvm_cv_target_os_type="Interix" ;;
  *-*-linux*)
    llvm_cv_target_os_type="Linux" ;;
  *-*-gnu*)
    llvm_cv_target_os_type="GNU" ;;
  *-*-solaris*)
    llvm_cv_target_os_type="SunOS" ;;
  *-*-auroraux*)
    llvm_cv_target_os_type="AuroraUX" ;;
  *-*-win32*)
    llvm_cv_target_os_type="Win32" ;;
  *-*-mingw*)
    llvm_cv_target_os_type="MingW" ;;
  *-*-haiku*)
    llvm_cv_target_os_type="Haiku" ;;
  *-*-rtems*)
    llvm_cv_target_os_type="RTEMS" ;;
  *-*-nacl*)
    llvm_cv_target_os_type="NativeClient" ;;
  *-unknown-eabi*)
    llvm_cv_target_os_type="Freestanding" ;;
  *)
    llvm_cv_target_os_type="Unknown" ;;
esac])

dnl Make sure we aren't attempting to configure for an unknown system
if test "$llvm_cv_os_type" = "Unknown" ; then
  AC_MSG_ERROR([Operating system is unknown, configure can't continue])
fi

dnl Set the "OS" Makefile variable based on the platform type so the
dnl makefile can configure itself to specific build hosts
AC_SUBST(OS,$llvm_cv_os_type)
AC_SUBST(HOST_OS,$llvm_cv_os_type)
AC_SUBST(TARGET_OS,$llvm_cv_target_os_type)

dnl Set the LINKALL and NOLINKALL Makefile variables based on the platform
AC_SUBST(LINKALL,$llvm_cv_link_all_option)
AC_SUBST(NOLINKALL,$llvm_cv_no_link_all_option)

dnl Set the "LLVM_ON_*" variables based on llvm_cv_platform_type
dnl This is used by lib/Support to determine the basic kind of implementation
dnl to use.
case $llvm_cv_platform_type in
  Unix)
    AC_DEFINE([LLVM_ON_UNIX],[1],[Define if this is Unixish platform])
    AC_SUBST(LLVM_ON_UNIX,[1])
    AC_SUBST(LLVM_ON_WIN32,[0])
    ;;
  Win32)
    AC_DEFINE([LLVM_ON_WIN32],[1],[Define if this is Win32ish platform])
    AC_SUBST(LLVM_ON_UNIX,[0])
    AC_SUBST(LLVM_ON_WIN32,[1])
    ;;
esac

dnl Determine what our target architecture is and configure accordingly.
dnl This will allow Makefiles to make a distinction between the hardware and
dnl the OS.
AC_CACHE_CHECK([target architecture],[llvm_cv_target_arch],
[case $target in
  i?86-*)                 llvm_cv_target_arch="x86" ;;
  amd64-* | x86_64-*)     llvm_cv_target_arch="x86_64" ;;
  sparc*-*)               llvm_cv_target_arch="Sparc" ;;
  powerpc*-*)             llvm_cv_target_arch="PowerPC" ;;
  arm*-*)                 llvm_cv_target_arch="ARM" ;;
  aarch64*-*)             llvm_cv_target_arch="AArch64" ;;
  mips-* | mips64-*)      llvm_cv_target_arch="Mips" ;;
  mipsel-* | mips64el-*)  llvm_cv_target_arch="Mips" ;;
  xcore-*)                llvm_cv_target_arch="XCore" ;;
  msp430-*)               llvm_cv_target_arch="MSP430" ;;
  hexagon-*)              llvm_cv_target_arch="Hexagon" ;;
  nvptx-*)                llvm_cv_target_arch="NVPTX" ;;
  s390x-*)                llvm_cv_target_arch="SystemZ" ;;
  *)                      llvm_cv_target_arch="Unknown" ;;
esac])

if test "$llvm_cv_target_arch" = "Unknown" ; then
  AC_MSG_WARN([Configuring LLVM for an unknown target archicture])
fi

# Determine the LLVM native architecture for the target
case "$llvm_cv_target_arch" in
    x86)     LLVM_NATIVE_ARCH="X86" ;;
    x86_64)  LLVM_NATIVE_ARCH="X86" ;;
    *)       LLVM_NATIVE_ARCH="$llvm_cv_target_arch" ;;
esac

dnl Define a substitution, ARCH, for the target architecture
AC_SUBST(ARCH,$llvm_cv_target_arch)

dnl Check for the endianness of the target
AC_C_BIGENDIAN(AC_SUBST([ENDIAN],[big]),AC_SUBST([ENDIAN],[little]))

dnl Check for build platform executable suffix if we're crosscompiling
if test "$cross_compiling" = yes; then
  AC_SUBST(LLVM_CROSS_COMPILING, [1])
  AC_BUILD_EXEEXT
  ac_build_prefix=${build_alias}-
  AC_CHECK_PROG(BUILD_CXX, ${ac_build_prefix}g++, ${ac_build_prefix}g++)
  if test -z "$BUILD_CXX"; then
     AC_CHECK_PROG(BUILD_CXX, g++, g++)
     if test -z "$BUILD_CXX"; then
       AC_CHECK_PROG(BUILD_CXX, c++, c++, , , /usr/ucb/c++)
     fi
  fi
else
  AC_SUBST(LLVM_CROSS_COMPILING, [0])
fi

dnl Check to see if there's a .svn or .git directory indicating that this
dnl build is being done from a checkout. This sets up several defaults for
dnl the command line switches. When we build with a checkout directory,
dnl we get a debug with assertions turned on. Without, we assume a source
dnl release and we get an optimized build without assertions.
dnl See --enable-optimized and --enable-assertions below
dnl Make sure we check LLVM root dir
llvmsrcdir="$srcdir/../../../.."
if test -d ".svn" -o -d "${llvmsrcdir}/.svn" -o -d ".git" -o -d "${llvmsrcdir}/.git"; then
   cvsbuild="yes"
   optimize="no"
   AC_SUBST(CVSBUILD,[[CVSBUILD=1]])
else
   cvsbuild="no"
   optimize="yes"
fi

dnl===-----------------------------------------------------------------------===
dnl===
dnl=== SECTION 3: Command line arguments for the configure script.
dnl===
dnl===-----------------------------------------------------------------------===

dnl --enable-libcpp : check whether or not to use libc++ on the command line
AC_ARG_ENABLE(libcpp,
              AS_HELP_STRING([--enable-libcpp],
                             [Use libc++ if available (default is NO)]),,
                             enableval=default)
case "$enableval" in
  yes) AC_SUBST(ENABLE_LIBCPP,[1]) ;;
  no)  AC_SUBST(ENABLE_LIBCPP,[0]) ;;
  default) AC_SUBST(ENABLE_LIBCPP,[0]);;
  *) AC_MSG_ERROR([Invalid setting for --enable-libcpp. Use "yes" or "no"]) ;;
esac

dnl --enable-cxx11 : check whether or not to use -std=c++11 on the command line
AC_ARG_ENABLE(cxx11,
              AS_HELP_STRING([--enable-cxx11],
                             [Use c++11 if available (default is NO)]),,
                             enableval=default)
case "$enableval" in
  yes) AC_SUBST(ENABLE_CXX11,[1]) ;;
  no)  AC_SUBST(ENABLE_CXX11,[0]) ;;
  default) AC_SUBST(ENABLE_CXX11,[0]);;
  *) AC_MSG_ERROR([Invalid setting for --enable-cxx11. Use "yes" or "no"]) ;;
esac

dnl --enable-optimized : check whether they want to do an optimized build:
AC_ARG_ENABLE(optimized, AS_HELP_STRING(
 --enable-optimized,[Compile with optimizations enabled (default is NO)]),,enableval=$optimize)
if test ${enableval} = "no" ; then
  AC_SUBST(ENABLE_OPTIMIZED,[[]])
else
  AC_SUBST(ENABLE_OPTIMIZED,[[ENABLE_OPTIMIZED=1]])
fi

dnl --enable-profiling : check whether they want to do a profile build:
AC_ARG_ENABLE(profiling, AS_HELP_STRING(
 --enable-profiling,[Compile with profiling enabled (default is NO)]),,enableval="no")
if test ${enableval} = "no" ; then
  AC_SUBST(ENABLE_PROFILING,[[]])
else
  AC_SUBST(ENABLE_PROFILING,[[ENABLE_PROFILING=1]])
fi

dnl --enable-assertions : check whether they want to turn on assertions or not:
AC_ARG_ENABLE(assertions,AS_HELP_STRING(
  --enable-assertions,[Compile with assertion checks enabled (default is YES)]),, enableval="yes")
if test ${enableval} = "yes" ; then
  AC_SUBST(DISABLE_ASSERTIONS,[[]])
else
  AC_SUBST(DISABLE_ASSERTIONS,[[DISABLE_ASSERTIONS=1]])
fi

dnl --enable-werror : check whether we want Werror on by default
AC_ARG_ENABLE(werror,AS_HELP_STRING(
  --enable-werror,[Compile with -Werror enabled (default is NO)]),, enableval="no")
case "$enableval" in
  yes) AC_SUBST(ENABLE_WERROR,[1]) ;;
  no)  AC_SUBST(ENABLE_WERROR,[0]) ;;
  default) AC_SUBST(ENABLE_WERROR,[0]);;
  *) AC_MSG_ERROR([Invalid setting for --enable-werror. Use "yes" or "no"]) ;;
esac

dnl --enable-expensive-checks : check whether they want to turn on expensive debug checks:
AC_ARG_ENABLE(expensive-checks,AS_HELP_STRING(
  --enable-expensive-checks,[Compile with expensive debug checks enabled (default is NO)]),, enableval="no")
if test ${enableval} = "yes" ; then
  AC_SUBST(ENABLE_EXPENSIVE_CHECKS,[[ENABLE_EXPENSIVE_CHECKS=1]])
  AC_SUBST(EXPENSIVE_CHECKS,[[yes]])
else
  AC_SUBST(ENABLE_EXPENSIVE_CHECKS,[[]])
  AC_SUBST(EXPENSIVE_CHECKS,[[no]])
fi

dnl --enable-debug-runtime : should runtime libraries have debug symbols?
AC_ARG_ENABLE(debug-runtime,
   AS_HELP_STRING(--enable-debug-runtime,[Build runtime libs with debug symbols (default is NO)]),,enableval=no)
if test ${enableval} = "no" ; then
  AC_SUBST(DEBUG_RUNTIME,[[]])
else
  AC_SUBST(DEBUG_RUNTIME,[[DEBUG_RUNTIME=1]])
fi

dnl --enable-debug-symbols : should even optimized compiler libraries
dnl have debug symbols?
AC_ARG_ENABLE(debug-symbols,
   AS_HELP_STRING(--enable-debug-symbols,[Build compiler with debug symbols (default is NO if optimization is on and YES if it's off)]),,enableval=no)
if test ${enableval} = "no" ; then
  AC_SUBST(DEBUG_SYMBOLS,[[]])
else
  AC_SUBST(DEBUG_SYMBOLS,[[DEBUG_SYMBOLS=1]])
fi

dnl --enable-jit: check whether they want to enable the jit
AC_ARG_ENABLE(jit,
  AS_HELP_STRING(--enable-jit,
                 [Enable Just In Time Compiling (default is YES)]),,
  enableval=default)
if test ${enableval} = "no"
then
  AC_SUBST(JIT,[[]])
else
  case "$llvm_cv_target_arch" in
    x86)         AC_SUBST(TARGET_HAS_JIT,1) ;;
    Sparc)       AC_SUBST(TARGET_HAS_JIT,0) ;;
    PowerPC)     AC_SUBST(TARGET_HAS_JIT,1) ;;
    x86_64)      AC_SUBST(TARGET_HAS_JIT,1) ;;
    ARM)         AC_SUBST(TARGET_HAS_JIT,1) ;;
    AArch64)     AC_SUBST(TARGET_HAS_JIT,0) ;;
    Mips)        AC_SUBST(TARGET_HAS_JIT,1) ;;
    XCore)       AC_SUBST(TARGET_HAS_JIT,0) ;;
    MSP430)      AC_SUBST(TARGET_HAS_JIT,0) ;;
    Hexagon)     AC_SUBST(TARGET_HAS_JIT,0) ;;
    NVPTX)       AC_SUBST(TARGET_HAS_JIT,0) ;;
    SystemZ)     AC_SUBST(TARGET_HAS_JIT,1) ;;
    *)           AC_SUBST(TARGET_HAS_JIT,0) ;;
  esac
fi

dnl Allow enablement of building and installing docs
AC_ARG_ENABLE(docs,
              AS_HELP_STRING([--enable-docs],
                             [Build documents (default is YES)]),,
                             enableval=default)
case "$enableval" in
  yes) AC_SUBST(ENABLE_DOCS,[1]) ;;
  no)  AC_SUBST(ENABLE_DOCS,[0]) ;;
  default) AC_SUBST(ENABLE_DOCS,[1]) ;;
  *) AC_MSG_ERROR([Invalid setting for --enable-docs. Use "yes" or "no"]) ;;
esac

dnl Allow enablement of doxygen generated documentation
AC_ARG_ENABLE(doxygen,
              AS_HELP_STRING([--enable-doxygen],
                             [Build doxygen documentation (default is NO)]),,
                             enableval=default)
case "$enableval" in
  yes) AC_SUBST(ENABLE_DOXYGEN,[1]) ;;
  no)  AC_SUBST(ENABLE_DOXYGEN,[0]) ;;
  default) AC_SUBST(ENABLE_DOXYGEN,[0]) ;;
  *) AC_MSG_ERROR([Invalid setting for --enable-doxygen. Use "yes" or "no"]) ;;
esac

dnl Allow disablement of threads
AC_ARG_ENABLE(threads,
              AS_HELP_STRING([--enable-threads],
                             [Use threads if available (default is YES)]),,
                             enableval=default)
case "$enableval" in
  yes) AC_SUBST(ENABLE_THREADS,[1]) ;;
  no)  AC_SUBST(ENABLE_THREADS,[0]) ;;
  default) AC_SUBST(ENABLE_THREADS,[1]) ;;
  *) AC_MSG_ERROR([Invalid setting for --enable-threads. Use "yes" or "no"]) ;;
esac
AC_DEFINE_UNQUOTED([ENABLE_THREADS],$ENABLE_THREADS,[Define if threads enabled])

dnl Allow disablement of pthread.h
AC_ARG_ENABLE(pthreads,
              AS_HELP_STRING([--enable-pthreads],
                             [Use pthreads if available (default is YES)]),,
                             enableval=default)
case "$enableval" in
  yes) AC_SUBST(ENABLE_PTHREADS,[1]) ;;
  no)  AC_SUBST(ENABLE_PTHREADS,[0]) ;;
  default) AC_SUBST(ENABLE_PTHREADS,[1]) ;;
  *) AC_MSG_ERROR([Invalid setting for --enable-pthreads. Use "yes" or "no"]) ;;
esac

dnl Allow disablement of zlib
AC_ARG_ENABLE(zlib,
              AS_HELP_STRING([--enable-zlib],
                             [Use zlib for compression/decompression if
                              available (default is YES)]),,
                              enableval=default)
case "$enableval" in
  yes) AC_SUBST(LLVM_ENABLE_ZLIB,[1]) ;;
  no)  AC_SUBST(LLVM_ENABLE_ZLIB,[0]) ;;
  default) AC_SUBST(LLVM_ENABLE_ZLIB,[1]) ;;
  *) AC_MSG_ERROR([Invalid setting for --enable-zlib. Use "yes" or "no"]) ;;
esac
AC_DEFINE_UNQUOTED([LLVM_ENABLE_ZLIB],$LLVM_ENABLE_ZLIB,
                   [Define if zlib is enabled])

dnl Allow building without position independent code
AC_ARG_ENABLE(pic,
  AS_HELP_STRING([--enable-pic],
                 [Build LLVM with Position Independent Code (default is YES)]),,
                 enableval=default)
case "$enableval" in
  yes) AC_SUBST(ENABLE_PIC,[1]) ;;
  no)  AC_SUBST(ENABLE_PIC,[0]) ;;
  default) AC_SUBST(ENABLE_PIC,[1]) ;;
  *) AC_MSG_ERROR([Invalid setting for --enable-pic. Use "yes" or "no"]) ;;
esac
AC_DEFINE_UNQUOTED([ENABLE_PIC],$ENABLE_PIC,
                   [Define if position independent code is enabled])

dnl Allow building a shared library and linking tools against it.
AC_ARG_ENABLE(shared,
  AS_HELP_STRING([--enable-shared],
                 [Build a shared library and link tools against it (default is NO)]),,
                 enableval=default)
case "$enableval" in
  yes) AC_SUBST(ENABLE_SHARED,[1]) ;;
  no)  AC_SUBST(ENABLE_SHARED,[0]) ;;
  default) AC_SUBST(ENABLE_SHARED,[0]) ;;
  *) AC_MSG_ERROR([Invalid setting for --enable-shared. Use "yes" or "no"]) ;;
esac

dnl Allow libstdc++ is embedded in LLVM.dll.
AC_ARG_ENABLE(embed-stdcxx,
  AS_HELP_STRING([--enable-embed-stdcxx],
                 [Build a shared library with embedded libstdc++ for Win32 DLL (default is YES)]),,
                 enableval=default)
case "$enableval" in
  yes) AC_SUBST(ENABLE_EMBED_STDCXX,[1]) ;;
  no)  AC_SUBST(ENABLE_EMBED_STDCXX,[0]) ;;
  default) AC_SUBST(ENABLE_EMBED_STDCXX,[1]) ;;
  *) AC_MSG_ERROR([Invalid setting for --enable-embed-stdcxx. Use "yes" or "no"]) ;;
esac

dnl Enable embedding timestamp information into build.
AC_ARG_ENABLE(timestamps,
  AS_HELP_STRING([--enable-timestamps],
                 [Enable embedding timestamp information in build (default is YES)]),,
                 enableval=default)
case "$enableval" in
  yes) AC_SUBST(ENABLE_TIMESTAMPS,[1]) ;;
  no)  AC_SUBST(ENABLE_TIMESTAMPS,[0]) ;;
  default) AC_SUBST(ENABLE_TIMESTAMPS,[1]) ;;
  *) AC_MSG_ERROR([Invalid setting for --enable-timestamps. Use "yes" or "no"]) ;;
esac
AC_DEFINE_UNQUOTED([ENABLE_TIMESTAMPS],$ENABLE_TIMESTAMPS,
                   [Define if timestamp information (e.g., __DATE___) is allowed])

dnl Allow specific targets to be specified for building (or not)
TARGETS_TO_BUILD=""
AC_ARG_ENABLE([targets],AS_HELP_STRING([--enable-targets],
    [Build specific host targets: all or target1,target2,... Valid targets are:
     host, x86, x86_64, sparc, powerpc, arm, aarch64, mips, hexagon,
     xcore, msp430, nvptx, systemz, r600, and cpp (default=all)]),,
    enableval=all)
if test "$enableval" = host-only ; then
  enableval=host
fi
case "$enableval" in
  all) TARGETS_TO_BUILD="X86 Sparc PowerPC AArch64 ARM Mips XCore MSP430 CppBackend NVPTX Hexagon SystemZ R600" ;;
  *)for a_target in `echo $enableval|sed -e 's/,/ /g' ` ; do
      case "$a_target" in
        x86)      TARGETS_TO_BUILD="X86 $TARGETS_TO_BUILD" ;;
        x86_64)   TARGETS_TO_BUILD="X86 $TARGETS_TO_BUILD" ;;
        sparc)    TARGETS_TO_BUILD="Sparc $TARGETS_TO_BUILD" ;;
        powerpc)  TARGETS_TO_BUILD="PowerPC $TARGETS_TO_BUILD" ;;
        aarch64)  TARGETS_TO_BUILD="AArch64 $TARGETS_TO_BUILD" ;;
        arm)      TARGETS_TO_BUILD="ARM $TARGETS_TO_BUILD" ;;
        mips)     TARGETS_TO_BUILD="Mips $TARGETS_TO_BUILD" ;;
        mipsel)   TARGETS_TO_BUILD="Mips $TARGETS_TO_BUILD" ;;
        mips64)   TARGETS_TO_BUILD="Mips $TARGETS_TO_BUILD" ;;
        mips64el) TARGETS_TO_BUILD="Mips $TARGETS_TO_BUILD" ;;
        xcore)    TARGETS_TO_BUILD="XCore $TARGETS_TO_BUILD" ;;
        msp430)   TARGETS_TO_BUILD="MSP430 $TARGETS_TO_BUILD" ;;
        cpp)      TARGETS_TO_BUILD="CppBackend $TARGETS_TO_BUILD" ;;
        hexagon)  TARGETS_TO_BUILD="Hexagon $TARGETS_TO_BUILD" ;;
        nvptx)    TARGETS_TO_BUILD="NVPTX $TARGETS_TO_BUILD" ;;
        systemz)  TARGETS_TO_BUILD="SystemZ $TARGETS_TO_BUILD" ;;
        r600)     TARGETS_TO_BUILD="R600 $TARGETS_TO_BUILD" ;;
        host) case "$llvm_cv_target_arch" in
            x86)         TARGETS_TO_BUILD="X86 $TARGETS_TO_BUILD" ;;
            x86_64)      TARGETS_TO_BUILD="X86 $TARGETS_TO_BUILD" ;;
            Sparc)       TARGETS_TO_BUILD="Sparc $TARGETS_TO_BUILD" ;;
            PowerPC)     TARGETS_TO_BUILD="PowerPC $TARGETS_TO_BUILD" ;;
            AArch64)     TARGETS_TO_BUILD="AArch64 $TARGETS_TO_BUILD" ;;
            ARM)         TARGETS_TO_BUILD="ARM $TARGETS_TO_BUILD" ;;
            Mips)        TARGETS_TO_BUILD="Mips $TARGETS_TO_BUILD" ;;
            XCore)       TARGETS_TO_BUILD="XCore $TARGETS_TO_BUILD" ;;
            MSP430)      TARGETS_TO_BUILD="MSP430 $TARGETS_TO_BUILD" ;;
            Hexagon)     TARGETS_TO_BUILD="Hexagon $TARGETS_TO_BUILD" ;;
            NVPTX)       TARGETS_TO_BUILD="NVPTX $TARGETS_TO_BUILD" ;;
            SystemZ)     TARGETS_TO_BUILD="SystemZ $TARGETS_TO_BUILD" ;;
            *)       AC_MSG_ERROR([Can not set target to build]) ;;
          esac ;;
        *) AC_MSG_ERROR([Unrecognized target $a_target]) ;;
      esac
  done
  ;;
esac
AC_SUBST(TARGETS_TO_BUILD,$TARGETS_TO_BUILD)

# Determine whether we are building LLVM support for the native architecture.
# If so, define LLVM_NATIVE_ARCH to that LLVM target.
for a_target in $TARGETS_TO_BUILD; do
  if test "$a_target" = "$LLVM_NATIVE_ARCH"; then
    AC_DEFINE_UNQUOTED(LLVM_NATIVE_ARCH, $LLVM_NATIVE_ARCH,
      [LLVM architecture name for the native architecture, if available])
    LLVM_NATIVE_TARGET="LLVMInitialize${LLVM_NATIVE_ARCH}Target"
    LLVM_NATIVE_TARGETINFO="LLVMInitialize${LLVM_NATIVE_ARCH}TargetInfo"
    LLVM_NATIVE_TARGETMC="LLVMInitialize${LLVM_NATIVE_ARCH}TargetMC"
    LLVM_NATIVE_ASMPRINTER="LLVMInitialize${LLVM_NATIVE_ARCH}AsmPrinter"
    if test -f ${srcdir}/lib/Target/${LLVM_NATIVE_ARCH}/AsmParser/Makefile ; then
      LLVM_NATIVE_ASMPARSER="LLVMInitialize${LLVM_NATIVE_ARCH}AsmParser"
    fi
    if test -f ${srcdir}/lib/Target/${LLVM_NATIVE_ARCH}/Disassembler/Makefile ; then
      LLVM_NATIVE_DISASSEMBLER="LLVMInitialize${LLVM_NATIVE_ARCH}Disassembler"
    fi
    AC_DEFINE_UNQUOTED(LLVM_NATIVE_TARGET, $LLVM_NATIVE_TARGET,
      [LLVM name for the native Target init function, if available])
    AC_DEFINE_UNQUOTED(LLVM_NATIVE_TARGETINFO, $LLVM_NATIVE_TARGETINFO,
      [LLVM name for the native TargetInfo init function, if available])
    AC_DEFINE_UNQUOTED(LLVM_NATIVE_TARGETMC, $LLVM_NATIVE_TARGETMC,
      [LLVM name for the native target MC init function, if available])
    AC_DEFINE_UNQUOTED(LLVM_NATIVE_ASMPRINTER, $LLVM_NATIVE_ASMPRINTER,
      [LLVM name for the native AsmPrinter init function, if available])
    if test -f ${srcdir}/lib/Target/${LLVM_NATIVE_ARCH}/AsmParser/Makefile ; then
      AC_DEFINE_UNQUOTED(LLVM_NATIVE_ASMPARSER, $LLVM_NATIVE_ASMPARSER,
       [LLVM name for the native AsmParser init function, if available])
    fi
    if test -f ${srcdir}/lib/Target/${LLVM_NATIVE_ARCH}/Disassembler/Makefile ; then
      AC_DEFINE_UNQUOTED(LLVM_NATIVE_DISASSEMBLER, $LLVM_NATIVE_DISASSEMBLER,
       [LLVM name for the native Disassembler init function, if available])
    fi
  fi
done

# Build the LLVM_TARGET and LLVM_... macros for Targets.def and the individual
# target feature def files.
LLVM_ENUM_TARGETS=""
LLVM_ENUM_ASM_PRINTERS=""
LLVM_ENUM_ASM_PARSERS=""
LLVM_ENUM_DISASSEMBLERS=""
for target_to_build in $TARGETS_TO_BUILD; do
  LLVM_ENUM_TARGETS="LLVM_TARGET($target_to_build) $LLVM_ENUM_TARGETS"
  if test -f ${srcdir}/lib/Target/${target_to_build}/*AsmPrinter.cpp ; then
    LLVM_ENUM_ASM_PRINTERS="LLVM_ASM_PRINTER($target_to_build) $LLVM_ENUM_ASM_PRINTERS";
  fi
  if test -f ${srcdir}/lib/Target/${target_to_build}/AsmParser/Makefile ; then
    LLVM_ENUM_ASM_PARSERS="LLVM_ASM_PARSER($target_to_build) $LLVM_ENUM_ASM_PARSERS";
  fi
  if test -f ${srcdir}/lib/Target/${target_to_build}/Disassembler/Makefile ; then
    LLVM_ENUM_DISASSEMBLERS="LLVM_DISASSEMBLER($target_to_build) $LLVM_ENUM_DISASSEMBLERS";
  fi
done
AC_SUBST(LLVM_ENUM_TARGETS)
AC_SUBST(LLVM_ENUM_ASM_PRINTERS)
AC_SUBST(LLVM_ENUM_ASM_PARSERS)
AC_SUBST(LLVM_ENUM_DISASSEMBLERS)

dnl Override the option to use for optimized builds.
AC_ARG_WITH(optimize-option,
  AS_HELP_STRING([--with-optimize-option],
                 [Select the compiler options to use for optimized builds]),,
                 withval=default)
AC_MSG_CHECKING([optimization flags])
case "$withval" in
  default)
    case "$llvm_cv_os_type" in
    FreeBSD) optimize_option=-O2 ;;
    MingW) optimize_option=-O2 ;;
    *)     optimize_option=-O3 ;;
    esac ;;
  *) optimize_option="$withval" ;;
esac
AC_SUBST(OPTIMIZE_OPTION,$optimize_option)
AC_MSG_RESULT([$optimize_option])

dnl Specify extra build options
AC_ARG_WITH(extra-options,
  AS_HELP_STRING([--with-extra-options],
                 [Specify additional options to compile LLVM with]),,
                 withval=default)
case "$withval" in
  default) EXTRA_OPTIONS= ;;
  *) EXTRA_OPTIONS=$withval ;;
esac
AC_SUBST(EXTRA_OPTIONS,$EXTRA_OPTIONS)

dnl Specify extra linker build options
AC_ARG_WITH(extra-ld-options,
  AS_HELP_STRING([--with-extra-ld-options],
                 [Specify additional options to link LLVM with]),,
                 withval=default)
case "$withval" in
  default) EXTRA_LD_OPTIONS= ;;
  *) EXTRA_LD_OPTIONS=$withval ;;
esac
AC_SUBST(EXTRA_LD_OPTIONS,$EXTRA_LD_OPTIONS)

dnl Allow specific bindings to be specified for building (or not)
AC_ARG_ENABLE([bindings],AS_HELP_STRING([--enable-bindings],
    [Build specific language bindings: all,auto,none,{binding-name} (default=auto)]),,
    enableval=default)
BINDINGS_TO_BUILD=""
case "$enableval" in
  yes | default | auto) BINDINGS_TO_BUILD="auto" ;;
  all ) BINDINGS_TO_BUILD="ocaml" ;;
  none | no) BINDINGS_TO_BUILD="" ;;
  *)for a_binding in `echo $enableval|sed -e 's/,/ /g' ` ; do
      case "$a_binding" in
        ocaml) BINDINGS_TO_BUILD="ocaml $BINDINGS_TO_BUILD" ;;
        *) AC_MSG_ERROR([Unrecognized binding $a_binding]) ;;
      esac
  done
  ;;
esac

dnl Allow the ocaml libdir to be overridden. This could go in a configure
dnl script for bindings/ocaml/configure, except that its auto value depends on
dnl OCAMLC, which is found here to support tests.
AC_ARG_WITH([ocaml-libdir],
  [AS_HELP_STRING([--with-ocaml-libdir],
    [Specify install location for ocaml bindings (default is stdlib)])],
  [],
  [withval=auto])
case "$withval" in
  auto) with_ocaml_libdir="$withval" ;;
  /* | [[A-Za-z]]:[[\\/]]*) with_ocaml_libdir="$withval" ;;
  *) AC_MSG_ERROR([Invalid path for --with-ocaml-libdir. Provide full path]) ;;
esac

AC_ARG_WITH(clang-resource-dir,
  AS_HELP_STRING([--with-clang-resource-dir],
    [Relative directory from the Clang binary for resource files]),,
    withval="")
AC_DEFINE_UNQUOTED(CLANG_RESOURCE_DIR,"$withval",
                   [Relative directory for resource files])

AC_ARG_WITH(c-include-dirs,
  AS_HELP_STRING([--with-c-include-dirs],
    [Colon separated list of directories clang will search for headers]),,
    withval="")
AC_DEFINE_UNQUOTED(C_INCLUDE_DIRS,"$withval",
                   [Directories clang will search for headers])

# Clang normally uses the system c++ headers and libraries. With this option,
# clang will use the ones provided by a gcc installation instead. This option should
# be passed the same value that was used with --prefix when configuring gcc.
AC_ARG_WITH(gcc-toolchain,
  AS_HELP_STRING([--with-gcc-toolchain],
    [Directory where gcc is installed.]),,
    withval="")
AC_DEFINE_UNQUOTED(GCC_INSTALL_PREFIX,"$withval",
                   [Directory where gcc is installed.])

dnl Allow linking of LLVM with GPLv3 binutils code.
AC_ARG_WITH(binutils-include,
  AS_HELP_STRING([--with-binutils-include],
    [Specify path to binutils/include/ containing plugin-api.h file for gold plugin.]),,
  withval=default)
case "$withval" in
  default) WITH_BINUTILS_INCDIR=default ;;
  /* | [[A-Za-z]]:[[\\/]]*)      WITH_BINUTILS_INCDIR=$withval ;;
  *) AC_MSG_ERROR([Invalid path for --with-binutils-include. Provide full path]) ;;
esac
if test "x$WITH_BINUTILS_INCDIR" != xdefault ; then
  AC_SUBST(BINUTILS_INCDIR,$WITH_BINUTILS_INCDIR)
  if test ! -f "$WITH_BINUTILS_INCDIR/plugin-api.h"; then
     echo "$WITH_BINUTILS_INCDIR/plugin-api.h"
     AC_MSG_ERROR([Invalid path to directory containing plugin-api.h.]);
  fi
fi

dnl Specify the URL where bug reports should be submitted.
AC_ARG_WITH(bug-report-url,
  AS_HELP_STRING([--with-bug-report-url],
    [Specify the URL where bug reports should be submitted (default=http://llvm.org/bugs/)]),,
    withval="http://llvm.org/bugs/")
AC_DEFINE_UNQUOTED(BUG_REPORT_URL,"$withval",
                   [Bug report URL.])

dnl --enable-terminfo: check whether the user wants to control use of terminfo:
AC_ARG_ENABLE(terminfo,AS_HELP_STRING(
  [--enable-terminfo],
  [Query the terminfo database if available (default is YES)]),
  [case "$enableval" in
    yes) llvm_cv_enable_terminfo="yes" ;;
    no)  llvm_cv_enable_terminfo="no"  ;;
    *) AC_MSG_ERROR([Invalid setting for --enable-terminfo. Use "yes" or "no"]) ;;
  esac],
  llvm_cv_enable_terminfo="yes")

dnl --enable-libffi : check whether the user wants to turn off libffi:
AC_ARG_ENABLE(libffi,AS_HELP_STRING(
  --enable-libffi,[Check for the presence of libffi (default is NO)]),
  [case "$enableval" in
    yes) llvm_cv_enable_libffi="yes" ;;
    no)  llvm_cv_enable_libffi="no"  ;;
    *) AC_MSG_ERROR([Invalid setting for --enable-libffi. Use "yes" or "no"]) ;;
  esac],
  llvm_cv_enable_libffi=no)

dnl===-----------------------------------------------------------------------===
dnl===
dnl=== SECTION 4: Check for programs we need and that they are the right version
dnl===
dnl===-----------------------------------------------------------------------===

AC_PROG_NM
AC_SUBST(NM)

dnl Check for the tools that the makefiles require
AC_CHECK_GNU_MAKE
AC_PROG_LN_S
AC_PATH_PROG(CMP, [cmp], [cmp])
AC_PATH_PROG(CP, [cp], [cp])
AC_PATH_PROG(DATE, [date], [date])
AC_PATH_PROG(FIND, [find], [find])
AC_PATH_PROG(GREP, [grep], [grep])
AC_PATH_PROG(MKDIR,[mkdir],[mkdir])
AC_PATH_PROG(MV,   [mv],   [mv])
AC_PATH_PROG(LD,   [ld],   [ld])
AC_PROG_LD_GNU
AC_PROG_RANLIB
AC_CHECK_TOOL(AR, ar, false)
AC_PATH_PROG(RM,   [rm],   [rm])
AC_PATH_PROG(SED,  [sed],  [sed])
AC_PATH_PROG(TAR,  [tar],  [gtar])
AC_PATH_PROG(BINPWD,[pwd],  [pwd])

dnl Looking for misc. graph plotting software
AC_PATH_PROG(GRAPHVIZ, [Graphviz], [echo Graphviz])
if test "$GRAPHVIZ" != "echo Graphviz" ; then
  AC_DEFINE([HAVE_GRAPHVIZ],[1],[Define if the Graphviz program is available])
  dnl If we're targeting for mingw we should emit windows paths, not msys
  if test "$llvm_cv_os_type" = "MingW" ; then
    GRAPHVIZ=`echo $GRAPHVIZ | sed 's/^\/\([[A-Za-z]]\)\//\1:\//' `
  fi
  AC_DEFINE_UNQUOTED([LLVM_PATH_GRAPHVIZ],"$GRAPHVIZ${EXEEXT}",
   [Define to path to Graphviz program if found or 'echo Graphviz' otherwise])
fi
AC_PATH_PROG(DOT, [dot], [echo dot])
if test "$DOT" != "echo dot" ; then
  AC_DEFINE([HAVE_DOT],[1],[Define if the dot program is available])
  dnl If we're targeting for mingw we should emit windows paths, not msy
  if test "$llvm_cv_os_type" = "MingW" ; then
    DOT=`echo $DOT | sed 's/^\/\([[A-Za-z]]\)\//\1:\//' `
  fi
  AC_DEFINE_UNQUOTED([LLVM_PATH_DOT],"$DOT${EXEEXT}",
   [Define to path to dot program if found or 'echo dot' otherwise])
fi
AC_PATH_PROG(FDP, [fdp], [echo fdp])
if test "$FDP" != "echo fdp" ; then
  AC_DEFINE([HAVE_FDP],[1],[Define if the neat program is available])
  dnl If we're targeting for mingw we should emit windows paths, not msys
  if test "$llvm_cv_os_type" = "MingW" ; then
    FDP=`echo $FDP | sed 's/^\/\([[A-Za-z]]\)\//\1:\//' `
  fi
  AC_DEFINE_UNQUOTED([LLVM_PATH_FDP],"$FDP${EXEEXT}",
   [Define to path to fdp program if found or 'echo fdp' otherwise])
fi
AC_PATH_PROG(NEATO, [neato], [echo neato])
if test "$NEATO" != "echo neato" ; then
  AC_DEFINE([HAVE_NEATO],[1],[Define if the neat program is available])
  dnl If we're targeting for mingw we should emit windows paths, not msys
  if test "$llvm_cv_os_type" = "MingW" ; then
    NEATO=`echo $NEATO | sed 's/^\/\([[A-Za-z]]\)\//\1:\//' `
  fi
  AC_DEFINE_UNQUOTED([LLVM_PATH_NEATO],"$NEATO${EXEEXT}",
   [Define to path to neato program if found or 'echo neato' otherwise])
fi
AC_PATH_PROG(TWOPI, [twopi], [echo twopi])
if test "$TWOPI" != "echo twopi" ; then
  AC_DEFINE([HAVE_TWOPI],[1],[Define if the neat program is available])
  dnl If we're targeting for mingw we should emit windows paths, not msys
  if test "$llvm_cv_os_type" = "MingW" ; then
    TWOPI=`echo $TWOPI | sed 's/^\/\([[A-Za-z]]\)\//\1:\//' `
  fi
  AC_DEFINE_UNQUOTED([LLVM_PATH_TWOPI],"$TWOPI${EXEEXT}",
   [Define to path to twopi program if found or 'echo twopi' otherwise])
fi
AC_PATH_PROG(CIRCO, [circo], [echo circo])
if test "$CIRCO" != "echo circo" ; then
  AC_DEFINE([HAVE_CIRCO],[1],[Define if the neat program is available])
  dnl If we're targeting for mingw we should emit windows paths, not msys
  if test "$llvm_cv_os_type" = "MingW" ; then
    CIRCO=`echo $CIRCO | sed 's/^\/\([[A-Za-z]]\)\//\1:\//' `
  fi
  AC_DEFINE_UNQUOTED([LLVM_PATH_CIRCO],"$CIRCO${EXEEXT}",
   [Define to path to circo program if found or 'echo circo' otherwise])
fi
AC_PATH_PROGS(GV, [gv gsview32], [echo gv])
if test "$GV" != "echo gv" ; then
  AC_DEFINE([HAVE_GV],[1],[Define if the gv program is available])
  dnl If we're targeting for mingw we should emit windows paths, not msys
  if test "$llvm_cv_os_type" = "MingW" ; then
    GV=`echo $GV | sed 's/^\/\([[A-Za-z]]\)\//\1:\//' `
  fi
  AC_DEFINE_UNQUOTED([LLVM_PATH_GV],"$GV${EXEEXT}",
   [Define to path to gv program if found or 'echo gv' otherwise])
fi
AC_PATH_PROG(DOTTY, [dotty], [echo dotty])
if test "$DOTTY" != "echo dotty" ; then
  AC_DEFINE([HAVE_DOTTY],[1],[Define if the dotty program is available])
  dnl If we're targeting for mingw we should emit windows paths, not msys
  if test "$llvm_cv_os_type" = "MingW" ; then
    DOTTY=`echo $DOTTY | sed 's/^\/\([[A-Za-z]]\)\//\1:\//' `
  fi
  AC_DEFINE_UNQUOTED([LLVM_PATH_DOTTY],"$DOTTY${EXEEXT}",
   [Define to path to dotty program if found or 'echo dotty' otherwise])
fi
AC_PATH_PROG(XDOT_PY, [xdot.py], [echo xdot.py])
if test "$XDOT_PY" != "echo xdot.py" ; then
  AC_DEFINE([HAVE_XDOT_PY],[1],[Define if the xdot.py program is available])
  dnl If we're targeting for mingw we should emit windows paths, not msys
  if test "$llvm_cv_os_type" = "MingW" ; then
    XDOT_PY=`echo $XDOT_PY | sed 's/^\/\([[A-Za-z]]\)\//\1:\//' `
  fi
  AC_DEFINE_UNQUOTED([LLVM_PATH_XDOT_PY],"$XDOT_PY${EXEEXT}",
   [Define to path to xdot.py program if found or 'echo xdot.py' otherwise])
fi

dnl Find the install program
AC_PROG_INSTALL
dnl Prepend src dir to install path dir if it's a relative path
dnl This is a hack for installs that take place in something other
dnl than the top level.
case "$INSTALL" in
 [[\\/$]]* | ?:[[\\/]]* ) ;;
 *)  INSTALL="\\\$(TOPSRCDIR)/$INSTALL" ;;
esac

dnl Checks for documentation and testing tools that we can do without. If these
dnl are not found then they are set to "true" which always succeeds but does
dnl nothing. This just lets the build output show that we could have done
dnl something if the tool was available.
AC_PATH_PROG(BZIP2, [bzip2])
AC_PATH_PROG(CAT, [cat])
AC_PATH_PROG(DOXYGEN, [doxygen])
AC_PATH_PROG(GROFF, [groff])
AC_PATH_PROG(GZIPBIN, [gzip])
AC_PATH_PROG(POD2HTML, [pod2html])
AC_PATH_PROG(POD2MAN, [pod2man])
AC_PATH_PROG(PDFROFF, [pdfroff])
AC_PATH_PROG(RUNTEST, [runtest])
DJ_AC_PATH_TCLSH
AC_PATH_PROG(ZIP, [zip])
AC_PATH_PROGS(OCAMLC, [ocamlc])
AC_PATH_PROGS(OCAMLOPT, [ocamlopt])
AC_PATH_PROGS(OCAMLDEP, [ocamldep])
AC_PATH_PROGS(OCAMLDOC, [ocamldoc])
AC_PATH_PROGS(GAS, [gas as])

dnl Get the version of the linker in use.
AC_LINK_GET_VERSION

dnl Determine whether the linker supports the -R option.
AC_LINK_USE_R

dnl Determine whether the compiler supports the -rdynamic option.
AC_LINK_EXPORT_DYNAMIC

dnl Determine whether the linker supports the --version-script option.
AC_LINK_VERSION_SCRIPT

dnl Check for libtool and the library that has dlopen function (which must come
dnl before the AC_PROG_LIBTOOL check in order to enable dlopening libraries with
dnl libtool).
AC_LIBTOOL_DLOPEN
AC_LIB_LTDL

AC_MSG_CHECKING([tool compatibility])

dnl Ensure that compilation tools are GCC or a GNU compatible compiler such as
dnl ICC; we use GCC specific options in the makefiles so the compiler needs
dnl to support those options.
dnl "icc" emits gcc signatures
dnl "icc -no-gcc" emits no gcc signature BUT is still compatible
ICC=no
IXX=no
case $CC in
  icc*|icpc*)
    ICC=yes
    IXX=yes
    ;;
   *)
    ;;
esac

if test "$GCC" != "yes" && test "$ICC" != "yes"
then
  AC_MSG_ERROR([gcc|icc required but not found])
fi

dnl Ensure that compilation tools are compatible with GCC extensions
if test "$GXX" != "yes" && test "$IXX" != "yes"
then
  AC_MSG_ERROR([g++|clang++|icc required but not found])
fi

dnl Verify that GCC is version 3.0 or higher
if test "$GCC" = "yes"
then
  AC_COMPILE_IFELSE([[#if !defined(__GNUC__) || __GNUC__ < 3
#error Unsupported GCC version
#endif
]], [], [AC_MSG_ERROR([gcc 3.x required, but you have a lower version])])
fi

dnl Check for GNU Make.  We use its extensions, so don't build without it
if test -z "$llvm_cv_gnu_make_command"
then
  AC_MSG_ERROR([GNU Make required but not found])
fi

dnl Tool compatibility is okay if we make it here.
AC_MSG_RESULT([ok])

dnl Check optional compiler flags.
AC_MSG_CHECKING([optional compiler flags])
CXX_FLAG_CHECK(NO_VARIADIC_MACROS, [-Wno-variadic-macros])
CXX_FLAG_CHECK(NO_MISSING_FIELD_INITIALIZERS, [-Wno-missing-field-initializers])
CXX_FLAG_CHECK(COVERED_SWITCH_DEFAULT, [-Wcovered-switch-default])
AC_MSG_RESULT([$NO_VARIADIC_MACROS $NO_MISSING_FIELD_INITIALIZERS $COVERED_SWITCH_DEFAULT])

dnl===-----------------------------------------------------------------------===
dnl===
dnl=== SECTION 5: Check for libraries
dnl===
dnl===-----------------------------------------------------------------------===

AC_CHECK_LIB(m,sin)
if test "$llvm_cv_os_type" = "MingW" ; then
  AC_CHECK_LIB(imagehlp, main)
  AC_CHECK_LIB(psapi, main)
  AC_CHECK_LIB(shell32, main)
fi

dnl dlopen() is required for plugin support.
AC_SEARCH_LIBS(dlopen,dl,AC_DEFINE([HAVE_DLOPEN],[1],
               [Define if dlopen() is available on this platform.]),
               AC_MSG_WARN([dlopen() not found - disabling plugin support]))

dnl Search for the clock_gettime() function. Note that we rely on the POSIX
dnl macros to detect whether clock_gettime is available, this just finds the
dnl right libraries to link with.
AC_SEARCH_LIBS(clock_gettime,rt)

dnl The curses library is optional; used for querying terminal info
if test "$llvm_cv_enable_terminfo" = "yes" ; then
  dnl We need the has_color functionality in curses for it to be useful.
  AC_SEARCH_LIBS(setupterm,tinfo curses ncurses ncursesw,
                 AC_DEFINE([HAVE_TERMINFO],[1],
                           [Define if the setupterm() function is supported this platform.]))
fi

dnl libffi is optional; used to call external functions from the interpreter
if test "$llvm_cv_enable_libffi" = "yes" ; then
  AC_SEARCH_LIBS(ffi_call,ffi,AC_DEFINE([HAVE_FFI_CALL],[1],
                 [Define if libffi is available on this platform.]),
                 AC_MSG_ERROR([libffi not found - configure without --enable-libffi to compile without it]))
fi

dnl mallinfo is optional; the code can compile (minus features) without it
AC_SEARCH_LIBS(mallinfo,malloc,AC_DEFINE([HAVE_MALLINFO],[1],
               [Define if mallinfo() is available on this platform.]))

dnl pthread locking functions are optional - but llvm will not be thread-safe
dnl without locks.
if test "$ENABLE_THREADS" -eq 1 && test "$ENABLE_PTHREADS" -eq 1 ; then
  AC_CHECK_LIB(pthread, pthread_mutex_init)
  AC_SEARCH_LIBS(pthread_mutex_lock,pthread,
                 AC_DEFINE([HAVE_PTHREAD_MUTEX_LOCK],[1],
                           [Have pthread_mutex_lock]))
  AC_SEARCH_LIBS(pthread_rwlock_init,pthread,
                 AC_DEFINE([HAVE_PTHREAD_RWLOCK_INIT],[1],
                 [Have pthread_rwlock_init]))
  AC_SEARCH_LIBS(pthread_getspecific,pthread,
                 AC_DEFINE([HAVE_PTHREAD_GETSPECIFIC],[1],
                 [Have pthread_getspecific]))
fi

dnl zlib is optional; used for compression/uncompression
if test "$LLVM_ENABLE_ZLIB" -eq 1 ; then
  AC_CHECK_LIB(z, compress2)
fi

dnl Allow extra x86-disassembler library
AC_ARG_WITH(udis86,
  AS_HELP_STRING([--with-udis86=<path>],
    [Use udis86 external x86 disassembler library]),
    [
      AC_SUBST(USE_UDIS86, [1])
      case "$withval" in
        /usr/lib|yes) ;;
        *) LDFLAGS="$LDFLAGS -L${withval}" ;;
      esac
      AC_CHECK_LIB(udis86, ud_init, [], [
        echo "Error! You need to have libudis86 around."
        exit -1
      ])
    ],
    AC_SUBST(USE_UDIS86, [0]))
AC_DEFINE_UNQUOTED([USE_UDIS86],$USE_UDIS86,
                   [Define if use udis86 library])

dnl Allow OProfile support for JIT output.
AC_ARG_WITH(oprofile,
  AS_HELP_STRING([--with-oprofile=<prefix>],
    [Tell OProfile >= 0.9.4 how to symbolize JIT output]),
    [
      AC_SUBST(USE_OPROFILE, [1])
      case "$withval" in
        /usr|yes) llvm_cv_oppath=/usr/lib/oprofile ;;
        no) llvm_cv_oppath=
            AC_SUBST(USE_OPROFILE, [0]) ;;
        *) llvm_cv_oppath="${withval}/lib/oprofile"
           CPPFLAGS="-I${withval}/include";;
      esac
      if test -n "$llvm_cv_oppath" ; then
        LIBS="$LIBS -L${llvm_cv_oppath} -Wl,-rpath,${llvm_cv_oppath}"
        dnl Work around http://bugs.debian.org/cgi-bin/bugreport.cgi?bug=537744:
        dnl libbfd is not included properly in libopagent in some Debian
        dnl versions.  If libbfd isn't found at all, we assume opagent works
        dnl anyway.
        AC_SEARCH_LIBS(bfd_init, bfd, [], [])
        AC_SEARCH_LIBS(op_open_agent, opagent, [], [
          echo "Error! You need to have libopagent around."
          exit -1
        ])
        AC_CHECK_HEADER([opagent.h], [], [
          echo "Error! You need to have opagent.h around."
          exit -1
          ])
      fi
    ],
    [
      AC_SUBST(USE_OPROFILE, [0])
    ])
AC_DEFINE_UNQUOTED([USE_OPROFILE],$USE_OPROFILE,
                   [Define if we have the oprofile JIT-support library])

dnl===-----------------------------------------------------------------------===
dnl===
dnl=== SECTION 6: Check for header files
dnl===
dnl===-----------------------------------------------------------------------===

dnl First, use autoconf provided macros for specific headers that we need
dnl We don't check for ancient stuff or things that are guaranteed to be there
dnl by the C++ standard. We always use the <cfoo> versions of <foo.h> C headers.
dnl Generally we're looking for POSIX headers.
AC_HEADER_DIRENT
AC_HEADER_MMAP_ANONYMOUS
AC_HEADER_STAT
AC_HEADER_SYS_WAIT
AC_HEADER_TIME

AC_CHECK_HEADERS([dlfcn.h execinfo.h fcntl.h inttypes.h limits.h link.h])
AC_CHECK_HEADERS([malloc.h setjmp.h signal.h stdint.h termios.h unistd.h])
AC_CHECK_HEADERS([utime.h windows.h])
AC_CHECK_HEADERS([sys/mman.h sys/param.h sys/resource.h sys/time.h sys/uio.h])
AC_CHECK_HEADERS([sys/types.h sys/ioctl.h malloc/malloc.h mach/mach.h])
AC_CHECK_HEADERS([valgrind/valgrind.h])
AC_CHECK_HEADERS([fenv.h])
if test "$ENABLE_THREADS" -eq 1 && test "$ENABLE_PTHREADS" -eq 1 ; then
  AC_CHECK_HEADERS(pthread.h,
                   AC_SUBST(HAVE_PTHREAD, 1),
                   AC_SUBST(HAVE_PTHREAD, 0))
else
  AC_SUBST(HAVE_PTHREAD, 0)
fi
if test "$LLVM_ENABLE_ZLIB" -eq 1 ; then
  AC_CHECK_HEADERS(zlib.h,
                   AC_SUBST(HAVE_LIBZ, 1),
                   AC_SUBST(HAVE_LIBZ, 0))
else
  AC_SUBST(HAVE_LIBZ, 0)
fi

dnl Try to find ffi.h.
if test "$llvm_cv_enable_libffi" = "yes" ; then
  AC_CHECK_HEADERS([ffi.h ffi/ffi.h])
fi

dnl Try to find Darwin specific crash reporting libraries.
AC_CHECK_HEADERS([CrashReporterClient.h])

dnl Try to find Darwin specific crash reporting global.
AC_MSG_CHECKING([__crashreporter_info__])
AC_LINK_IFELSE(
  AC_LANG_SOURCE(
    [[extern const char *__crashreporter_info__;
      int main() {
        __crashreporter_info__ = "test";
        return 0;
      }
    ]]),
  AC_MSG_RESULT(yes)
  AC_DEFINE(HAVE_CRASHREPORTER_INFO, 1, Can use __crashreporter_info__),
  AC_MSG_RESULT(no)
  AC_DEFINE(HAVE_CRASHREPORTER_INFO, 0,
            Define if __crashreporter_info__ exists.))

dnl===-----------------------------------------------------------------------===
dnl===
dnl=== SECTION 7: Check for types and structures
dnl===
dnl===-----------------------------------------------------------------------===

AC_HUGE_VAL_CHECK
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_DEFINE_UNQUOTED([RETSIGTYPE],[void],[Define as the return type of signal handlers (`int' or `void').])
AC_STRUCT_TM
AC_CHECK_TYPES([int64_t],,AC_MSG_ERROR([Type int64_t required but not found]))
AC_CHECK_TYPES([uint64_t],,
         AC_CHECK_TYPES([u_int64_t],,
         AC_MSG_ERROR([Type uint64_t or u_int64_t required but not found])))

dnl===-----------------------------------------------------------------------===
dnl===
dnl=== SECTION 8: Check for specific functions needed
dnl===
dnl===-----------------------------------------------------------------------===

AC_CHECK_FUNCS([backtrace ceilf floorf roundf rintf nearbyintf getcwd ])
AC_CHECK_FUNCS([powf fmodf strtof round ])
AC_CHECK_FUNCS([getpagesize getrusage getrlimit setrlimit gettimeofday ])
AC_CHECK_FUNCS([isatty mkdtemp mkstemp ])
AC_CHECK_FUNCS([mktemp posix_spawn realpath sbrk setrlimit strdup ])
AC_CHECK_FUNCS([strerror strerror_r setenv ])
AC_CHECK_FUNCS([strtoll strtoq sysconf malloc_zone_statistics ])
AC_CHECK_FUNCS([setjmp longjmp sigsetjmp siglongjmp writev])
AC_C_PRINTF_A
AC_FUNC_RAND48

dnl Check the declaration "Secure API" on Windows environments.
AC_CHECK_DECLS([strerror_s])

dnl Check symbols in libgcc.a for JIT on Mingw.
if test "$llvm_cv_os_type" = "MingW" ; then
  AC_CHECK_LIB(gcc,_alloca,AC_DEFINE([HAVE__ALLOCA],[1],[Have host's _alloca]))
  AC_CHECK_LIB(gcc,__alloca,AC_DEFINE([HAVE___ALLOCA],[1],[Have host's __alloca]))
  AC_CHECK_LIB(gcc,__chkstk,AC_DEFINE([HAVE___CHKSTK],[1],[Have host's __chkstk]))
  AC_CHECK_LIB(gcc,___chkstk,AC_DEFINE([HAVE____CHKSTK],[1],[Have host's ___chkstk]))

  AC_CHECK_LIB(gcc,__ashldi3,AC_DEFINE([HAVE___ASHLDI3],[1],[Have host's __ashldi3]))
  AC_CHECK_LIB(gcc,__ashrdi3,AC_DEFINE([HAVE___ASHRDI3],[1],[Have host's __ashrdi3]))
  AC_CHECK_LIB(gcc,__divdi3,AC_DEFINE([HAVE___DIVDI3],[1],[Have host's __divdi3]))
  AC_CHECK_LIB(gcc,__fixdfdi,AC_DEFINE([HAVE___FIXDFDI],[1],[Have host's __fixdfdi]))
  AC_CHECK_LIB(gcc,__fixsfdi,AC_DEFINE([HAVE___FIXSFDI],[1],[Have host's __fixsfdi]))
  AC_CHECK_LIB(gcc,__floatdidf,AC_DEFINE([HAVE___FLOATDIDF],[1],[Have host's __floatdidf]))
  AC_CHECK_LIB(gcc,__lshrdi3,AC_DEFINE([HAVE___LSHRDI3],[1],[Have host's __lshrdi3]))
  AC_CHECK_LIB(gcc,__moddi3,AC_DEFINE([HAVE___MODDI3],[1],[Have host's __moddi3]))
  AC_CHECK_LIB(gcc,__udivdi3,AC_DEFINE([HAVE___UDIVDI3],[1],[Have host's __udivdi3]))
  AC_CHECK_LIB(gcc,__umoddi3,AC_DEFINE([HAVE___UMODDI3],[1],[Have host's __umoddi3]))

  AC_CHECK_LIB(gcc,__main,AC_DEFINE([HAVE___MAIN],[1],[Have host's __main]))
  AC_CHECK_LIB(gcc,__cmpdi2,AC_DEFINE([HAVE___CMPDI2],[1],[Have host's __cmpdi2]))
fi

dnl Check Win32 API EnumerateLoadedModules.
if test "$llvm_cv_os_type" = "MingW" ; then
  AC_MSG_CHECKING([whether EnumerateLoadedModules() accepts new decl])
  AC_COMPILE_IFELSE([[#include <windows.h>
#include <imagehlp.h>
extern void foo(PENUMLOADED_MODULES_CALLBACK);
extern void foo(BOOL(CALLBACK*)(PCSTR,ULONG_PTR,ULONG,PVOID));]],
[
  AC_MSG_RESULT([yes])
  llvm_cv_win32_elmcb_pcstr="PCSTR"
],
[
  AC_MSG_RESULT([no])
  llvm_cv_win32_elmcb_pcstr="PSTR"
])
  AC_DEFINE_UNQUOTED([WIN32_ELMCB_PCSTR],$llvm_cv_win32_elmcb_pcstr,[Type of 1st arg on ELM Callback])
fi

dnl Check for variations in the Standard C++ library and STL. These macros are
dnl provided by LLVM in the autoconf/m4 directory.
AC_FUNC_ISNAN
AC_FUNC_ISINF

dnl Check for mmap support.We also need to know if /dev/zero is required to
dnl be opened for allocating RWX memory.
dnl Make sure we aren't attempting to configure for an unknown system
if test "$llvm_cv_platform_type" = "Unix" ; then
  AC_FUNC_MMAP
  AC_FUNC_MMAP_FILE
  AC_NEED_DEV_ZERO_FOR_MMAP

  if test "$ac_cv_func_mmap_fixed_mapped" = "no"
  then
    AC_MSG_WARN([mmap() of a fixed address required but not supported])
  fi
  if test "$ac_cv_func_mmap_file" = "no"
  then
    AC_MSG_WARN([mmap() of files required but not found])
  fi
fi

dnl atomic builtins are required for threading support.
AC_MSG_CHECKING(for GCC atomic builtins)
dnl Since we'll be using these atomic builtins in C++ files we should test
dnl the C++ compiler.
AC_LANG_PUSH([C++])
AC_LINK_IFELSE(
  AC_LANG_SOURCE(
    [[int main() {
        volatile unsigned long val = 1;
        __sync_synchronize();
        __sync_val_compare_and_swap(&val, 1, 0);
        __sync_add_and_fetch(&val, 1);
        __sync_sub_and_fetch(&val, 1);
        return 0;
      }
    ]]),
  AC_LANG_POP([C++])
  AC_MSG_RESULT(yes)
  AC_DEFINE(LLVM_HAS_ATOMICS, 1, Has gcc/MSVC atomic intrinsics),
  AC_MSG_RESULT(no)
  AC_DEFINE(LLVM_HAS_ATOMICS, 0, Has gcc/MSVC atomic intrinsics)
  AC_MSG_WARN([LLVM will be built thread-unsafe because atomic builtins are missing]))

dnl===-----------------------------------------------------------------------===
dnl===
dnl=== SECTION 9: Additional checks, variables, etc.
dnl===
dnl===-----------------------------------------------------------------------===

dnl Handle 32-bit linux systems running a 64-bit kernel.
dnl This has to come after section 4 because it invokes the compiler.
if test "$llvm_cv_os_type" = "Linux" -a "$llvm_cv_target_arch" = "x86_64" ; then
  AC_IS_LINUX_MIXED
  if test "$llvm_cv_linux_mixed" = "yes"; then
    llvm_cv_target_arch="x86"
    ARCH="x86"
  fi
fi

dnl Check whether __dso_handle is present
AC_CHECK_FUNCS([__dso_handle])

dnl Propagate the shared library extension that the libltdl checks did to
dnl the Makefiles so we can use it there too
AC_SUBST(SHLIBEXT,$libltdl_cv_shlibext)

dnl Propagate the run-time library path variable that the libltdl
dnl checks found to the Makefiles so we can use it there too
AC_SUBST(SHLIBPATH_VAR,$libltdl_cv_shlibpath_var)

# Translate the various configuration directories and other basic
# information into substitutions that will end up in Makefile.config.in
# that these configured values can be used by the makefiles
if test "${prefix}" = "NONE" ; then
  prefix="/usr/local"
fi
eval LLVM_PREFIX="${prefix}";
eval LLVM_BINDIR="${prefix}/bin";
eval LLVM_DATADIR="${prefix}/share/llvm";
eval LLVM_DOCSDIR="${prefix}/share/doc/llvm";
eval LLVM_ETCDIR="${prefix}/etc/llvm";
eval LLVM_INCLUDEDIR="${prefix}/include";
eval LLVM_INFODIR="${prefix}/info";
eval LLVM_MANDIR="${prefix}/man";
LLVM_CONFIGTIME=`date`
AC_SUBST(LLVM_PREFIX)
AC_SUBST(LLVM_BINDIR)
AC_SUBST(LLVM_DATADIR)
AC_SUBST(LLVM_DOCSDIR)
AC_SUBST(LLVM_ETCDIR)
AC_SUBST(LLVM_INCLUDEDIR)
AC_SUBST(LLVM_INFODIR)
AC_SUBST(LLVM_MANDIR)
AC_SUBST(LLVM_CONFIGTIME)

# Place the various directores into the config.h file as #defines so that we
# can know about the installation paths within LLVM.
AC_DEFINE_UNQUOTED(LLVM_PREFIX,"$LLVM_PREFIX",
                   [Installation prefix directory])
AC_DEFINE_UNQUOTED(LLVM_BINDIR, "$LLVM_BINDIR",
                   [Installation directory for binary executables])
AC_DEFINE_UNQUOTED(LLVM_DATADIR, "$LLVM_DATADIR",
                   [Installation directory for data files])
AC_DEFINE_UNQUOTED(LLVM_DOCSDIR, "$LLVM_DOCSDIR",
                   [Installation directory for documentation])
AC_DEFINE_UNQUOTED(LLVM_ETCDIR, "$LLVM_ETCDIR",
                   [Installation directory for config files])
AC_DEFINE_UNQUOTED(LLVM_INCLUDEDIR, "$LLVM_INCLUDEDIR",
                   [Installation directory for include files])
AC_DEFINE_UNQUOTED(LLVM_INFODIR, "$LLVM_INFODIR",
                   [Installation directory for .info files])
AC_DEFINE_UNQUOTED(LLVM_MANDIR, "$LLVM_MANDIR",
                   [Installation directory for man pages])
AC_DEFINE_UNQUOTED(LLVM_CONFIGTIME, "$LLVM_CONFIGTIME",
                   [Time at which LLVM was configured])
AC_DEFINE_UNQUOTED(LLVM_DEFAULT_TARGET_TRIPLE, "$target",
                   [Target triple LLVM will generate code for by default])

# Determine which bindings to build.
if test "$BINDINGS_TO_BUILD" = auto ; then
  BINDINGS_TO_BUILD=""
  if test "x$OCAMLC" != x -a "x$OCAMLDEP" != x ; then
    BINDINGS_TO_BUILD="ocaml $BINDINGS_TO_BUILD"
  fi
fi
AC_SUBST(BINDINGS_TO_BUILD,$BINDINGS_TO_BUILD)

# This isn't really configurey, but it avoids having to repeat the list in
# other files.
AC_SUBST(ALL_BINDINGS,ocaml)

# Do any work necessary to ensure that bindings have what they need.
binding_prereqs_failed=0
for a_binding in $BINDINGS_TO_BUILD ; do
  case "$a_binding" in
  ocaml)
    if test "x$OCAMLC" = x ; then
      AC_MSG_WARN([--enable-bindings=ocaml specified, but ocamlc not found. Try configure OCAMLC=/path/to/ocamlc])
      binding_prereqs_failed=1
    fi
    if test "x$OCAMLDEP" = x ; then
      AC_MSG_WARN([--enable-bindings=ocaml specified, but ocamldep not found. Try configure OCAMLDEP=/path/to/ocamldep])
      binding_prereqs_failed=1
    fi
    if test "x$OCAMLOPT" = x ; then
      AC_MSG_WARN([--enable-bindings=ocaml specified, but ocamlopt not found. Try configure OCAMLOPT=/path/to/ocamlopt])
      dnl ocamlopt is optional!
    fi
    if test "x$with_ocaml_libdir" != xauto ; then
      AC_SUBST(OCAML_LIBDIR,$with_ocaml_libdir)
    else
      ocaml_stdlib="`"$OCAMLC" -where`"
      if test "$LLVM_PREFIX" '<' "$ocaml_stdlib" -a "$ocaml_stdlib" '<' "$LLVM_PREFIX~"
      then
        # ocaml stdlib is beneath our prefix; use stdlib
        AC_SUBST(OCAML_LIBDIR,$ocaml_stdlib)
      else
        # ocaml stdlib is outside our prefix; use libdir/ocaml
        AC_SUBST(OCAML_LIBDIR,${prefix}/lib/ocaml)
      fi
    fi
    ;;
  esac
done
if test "$binding_prereqs_failed" = 1 ; then
  AC_MSG_ERROR([Prequisites for bindings not satisfied. Fix them or use configure --disable-bindings.])
fi

dnl Determine whether the compiler supports -fvisibility-inlines-hidden.
AC_CXX_USE_VISIBILITY_INLINES_HIDDEN

dnl Determine linker rpath flag
if test "$llvm_cv_link_use_r" = "yes" ; then
  RPATH="-Wl,-R"
else
  RPATH="-Wl,-rpath"
fi
AC_SUBST(RPATH)

dnl Determine linker rdynamic flag
if test "$llvm_cv_link_use_export_dynamic" = "yes" ; then
  RDYNAMIC="-rdynamic"
else
  RDYNAMIC=""
fi
AC_SUBST(RDYNAMIC)

dnl===-----------------------------------------------------------------------===
dnl===
dnl=== SECTION 10: Specify the output files and generate it
dnl===
dnl===-----------------------------------------------------------------------===

dnl **************************************************************************
dnl End LLVM configure.ac Import
dnl **************************************************************************

dnl **************************************************************************
dnl Clang UPC runtime
dnl   Code above derived from LLVM sample project.
dnl **************************************************************************

AC_ARG_WITH(upc-pts,
  AS_HELP_STRING([--with-upc-pts],
    [UPC packed or struct pointer representation.]),,
    withval="packed")
  upc_pts=$withval
AC_DEFINE_UNQUOTED(UPC_PTS,"$withval",
                   [Choose the default representation of a UPC pointer-to-shared])

# Perl is required for checking packed pts representation
AC_CHECK_PROG(PERL, perl, perl)

AC_ARG_WITH([upc-pts-packed-bits],
  AS_HELP_STRING([--with-upc-pts-packed-bits],
    [Choose bit distribution in packed UPC pointer-to-shared representation]),
[
  if test x"$upc_pts" = x"packed"; then
    UPC_PTS_PHASE_SIZE=`echo "$withval" | $PERL -ne 'm/^(\d+),(\d+),(\d+)$/;
						  print $1 if ($1+$2+$3 == 64) && $1>=1;'`
    UPC_PTS_THREAD_SIZE=`echo "$withval" | $PERL -ne 'm/^(\d+),(\d+),(\d+)$/;
						  print $2 if ($1+$2+$3 == 64) && $2>=1;'`
    UPC_PTS_VADDR_SIZE=`echo "$withval" | $PERL -ne 'm/^(\d+),(\d+),(\d+)$/;
						  print $3 if ($1+$2+$3 == 64) && $3>=20;'`
    if test -z "$UPC_PTS_PHASE_SIZE" \
       || test -z "$UPC_PTS_THREAD_SIZE" \
       || test -z "$UPC_PTS_VADDR_SIZE" ; then
      AC_MSG_ERROR([--with-upc-pts-packed-bits=phase,thread,vaddr requires
  phase+thread+vaddr=64 and phase >= 1 and thread >= 1 and vaddr >= 20])
    fi
    AC_DEFINE_UNQUOTED(UPC_PTS_PACKED_BITS,"$withval",
                       [UPC packed pointer representation.])
  else
    AC_MSG_ERROR([--with-upc-pts-packed-bits is only supported for
the packed UPC pointer-to-shared representation])
  fi
],
[
  UPC_PTS_PHASE_SIZE=20
  UPC_PTS_THREAD_SIZE=10
  UPC_PTS_VADDR_SIZE=34
])

AC_ARG_WITH(upc-pts-vaddr-order,
  AS_HELP_STRING([--with-upc-pts-vaddr-order],
    [Choose position of the address field in UPC pointer-to-shared representation]),,
    withval="first")
  upc_vaddr_order=$withval
AC_DEFINE_UNQUOTED(UPC_VADDR_ORDER,"$withval",
                   [UPC vaddr first or last in the pointer representation.])

# Need to know the target size (32 or 64 bits)
AC_LINK_IFELSE([
#if __SIZEOF_POINTER__ != 8
error "32 bit target"
#endif
int main (){}
],
[target64=yes],
[target64=no])

AC_MSG_CHECKING([UPC shared pointer representation])
if test "x${upc_vaddr_order}" = xfirst; then
  GUPCR_PTS_VADDR_FIRST=1
else
  GUPCR_PTS_VADDR_FIRST=0
fi
if test "x${upc_pts}" = xpacked; then
  GUPCR_PTS_REP="packed"
  GUPCR_PTS_PACKED_REP=1
  GUPCR_PTS_VADDR_SIZE=$UPC_PTS_VADDR_SIZE
  GUPCR_PTS_PHASE_SIZE=$UPC_PTS_PHASE_SIZE
  GUPCR_PTS_THREAD_SIZE=$UPC_PTS_THREAD_SIZE
else
  GUPCR_PTS_REP="struct"
  if test "x${target64}" = xyes; then
    GUPCR_PTS_ALIGN=128
    GUPCR_PTS_VADDR_SIZE=64
    GUPCR_PTS_PHASE_SIZE=32
    GUPCR_PTS_THREAD_SIZE=32
    GUPCR_PTS_PHASE_TYPE="u_intSI_t"
    GUPCR_PTS_THREAD_TYPE="u_intSI_t"
  else
    GUPCR_PTS_ALIGN=64
    GUPCR_PTS_VADDR_SIZE=32
    GUPCR_PTS_PHASE_SIZE=16
    GUPCR_PTS_THREAD_SIZE=16
    GUPCR_PTS_PHASE_TYPE="u_intHI_t"
    GUPCR_PTS_THREAD_TYPE="u_intHI_t"
  fi
  GUPCR_PTS_VADDR_TYPE="void *"
fi
AC_MSG_RESULT($GUPCR_PTS_PHASE_SIZE-$GUPCR_PTS_THREAD_SIZE-$GUPCR_PTS_VADDR_SIZE)

# To match the cmake configuration, provide various runtime
# parameters via compiler options instead of changes to the
# config.h file.
AC_SUBST(GUPCR_PTS_PHASE_SIZE)
AC_SUBST(GUPCR_PTS_THREAD_SIZE)
AC_SUBST(GUPCR_PTS_VADDR_SIZE)
AC_SUBST(GUPCR_PTS_PACKED_REP)
AC_SUBST(GUPCR_PTS_STRUCT_REP)
AC_SUBST(GUPCR_PTS_VADDR_FIRST)
AC_SUBST(GUPCR_PTS_VADDR_TYPE)
AC_SUBST(GUPCR_PTS_THREAD_TYPE)
AC_SUBST(GUPCR_PTS_PHASE_TYPE)
AC_SUBST(GUPCR_PTS_ALIGN)

# Check for __sync_val_compare_and_swap
AC_CACHE_CHECK([whether the target supports __sync_*_compare_and_swap],
[libgupc_cv_have_sync_builtins],[
AC_LINK_IFELSE([
  AC_LANG_SOURCE([[
int foo; int main () {(void) __sync_val_compare_and_swap(&foo, 0, 1);}
  ]])
],
[libgupc_cv_have_sync_builtins=yes],
[libgupc_cv_have_sync_builtins=no])])
if test $libgupc_cv_have_sync_builtins = yes; then
  AC_DEFINE(HAVE_SYNC_BUILTINS, 1,
	    [Define to 1 if the target supports __sync_*_compare_and_swap.])
fi

# Check for __sync_fetch_and_add
AC_CACHE_CHECK([for __sync_fetch_and_add_4],
[upc_cv_sync_fetch_and_add_4], [
AC_LINK_IFELSE([
typedef unsigned int uint32  __attribute__ ((mode (SI)));
uint32 i;
int main() { return __sync_fetch_and_add (&i, 1); }
],
[upc_cv_sync_fetch_and_add_4=yes],
[upc_cv_sync_fetch_and_add_4=no])])
if test "$upc_cv_sync_fetch_and_add_4" = "yes"; then
  AC_DEFINE(HAVE_SYNC_FETCH_AND_ADD_4, 1,
    [Define to 1 if the compiler provides the
    __sync_fetch_and_add function for uint32])
fi

AC_CACHE_CHECK([for __sync_fetch_and_add_8],
[upc_cv_sync_fetch_and_add_8],
[AC_LINK_IFELSE([
  AC_LANG_SOURCE([[
typedef unsigned int uint64  __attribute__ ((mode (DI)));
uint64 i;
int main() { return __sync_fetch_and_add (&i, 1); }
  ]])
],
[upc_cv_sync_fetch_and_add_8=yes],
[upc_cv_sync_fetch_and_add_8=no])])
if test "$upc_cv_sync_fetch_and_add_8" = "yes"; then
  AC_DEFINE(HAVE_SYNC_FETCH_AND_ADD_8, 1,
    [Define to 1 if the compiler provides the
    __sync_fetch_and_add function for uint64])
fi

dnl Select target specific runtime implementation.

AC_MSG_CHECKING([for UPC runtime model])
AC_ARG_WITH(upc-runtime,
[AS_HELP_STRING([--with-upc-runtime=MODEL],
		[Specify the runtime implementation model for UPC,
		 where MODEL may be: 'SMP' (Symmetric Multiprocessing),
		 'Portals4', or "Fabric'.  [default='SMP']])],
[
  case "$withval" in
changequote(,)dnl
    [Ss][Mm][Pp])
      upc_runtime_model=smp
      ;;
    [Pp][Oo][Rr][Tt][Aa][Ll][Ss]4)
      upc_runtime_model=portals4
      ;;
    [Ff][Aa][Bb][Rr][Ii][Cc])
      upc_runtime_model=fabric
      ;;
changequote([,])dnl
    *)
      AC_MSG_ERROR([$withval is an invalid runtime model for
      --with-upc-runtime])
      ;;
  esac
],
[
  upc_runtime_model=smp
])
AC_MSG_RESULT([$upc_runtime_model])
LIBUPC_RUNTIME_MODEL=$upc_runtime_model
AC_SUBST(LIBUPC_RUNTIME_MODEL)

AC_MSG_CHECKING([for UPC runtime checking support])
AC_ARG_ENABLE(upc-runtime-checks,
[AS_HELP_STRING([--enable-upc-runtime-checks],
		[Enable internal UPC runtime checks that validate
		 arguments, and check for inconsistent runtime state
		 [default=no]])],
[
  case $enableval in
  yes | no) ;;
  *)
    AC_MSG_ERROR([--enable-upc-runtime-checks accepts only yes or no.])
  esac
],
[
  enable_upc_runtime_checks=no
])
AC_MSG_RESULT($enable_upc_runtime_checks)
if test "$enable_upc_runtime_checks" = "yes"; then
  AC_DEFINE(GUPCR_HAVE_CHECKS, 1,
            [Define to 1 if UPC runtime checks are supported.])
fi

AC_MSG_CHECKING([for UPC runtime statistics support])
AC_ARG_ENABLE(upc-runtime-stats,
[AS_HELP_STRING([--enable-upc-runtime-stats],
		[Enable internal UPC runtime statistics collection support;
		 these statistics count the number of various significant
		 internal operations, and dump those counts into a
		 per-process statistics file
		 [default=no]])],
[
  case $enableval in
  yes | no) ;;
  *)
    AC_MSG_ERROR([--enable-upc-runtime-stats accepts only yes or no.])
  esac
],
[
  enable_upc_runtime_stats=no
])
AC_MSG_RESULT($enable_upc_runtime_stats)
if test "$enable_upc_runtime_stats" = "yes"; then
  AC_DEFINE(GUPCR_HAVE_STATS, 1,
            [Define to 1 if UPC runtime statistics collection is supported.])
fi

AC_MSG_CHECKING([for UPC runtime trace support])
AC_ARG_ENABLE(upc-runtime-trace,
[AS_HELP_STRING([--enable-upc-runtime-trace],
		[Enable internal UPC runtime trace collection support;
		 a runtime trace is a time stamped log that records
		 various significant internal events; this trace
		 is written to a per-process log file
		 [default=no]])],
[
  case $enableval in
  yes | no) ;;
  *)
    AC_MSG_ERROR([--enable-upc-runtime-trace accepts only yes or no.])
  esac
],
[
  enable_upc_runtime_trace=no
])
AC_MSG_RESULT($enable_upc_runtime_trace)
if test "$enable_upc_runtime_trace" = "yes"; then
  AC_DEFINE(GUPCR_HAVE_TRACE, 1,
            [Define to 1 if UPC runtime tracing is supported.])
fi

AC_MSG_CHECKING([for UPC runtime debug configuration])
AC_ARG_ENABLE(upc-runtime-debug,
[AS_HELP_STRING([--enable-upc-runtime-debug],
		[Enable UPC runtime debugging mode,
		 where more expensive internal checks are implemented,
		 and conservative algorithms are used that
		 reduce the degree of parallelism, and that
		 exercise less complex/sophisticated operations
		 provided by the operating system and/or the
		 network communication packages called by the UPC runtime.
		 In addition, conservative compilation options will
		 be used to build the runtime, and debugging symbols
		 will be generated.
		 [default=no]])],
[
  case $enableval in
  yes | no) ;;
  *)
    AC_MSG_ERROR([--enable-upc-runtime-debug accepts only yes or no.])
  esac
],
[
  enable_upc_runtime_debug=no
])
AC_MSG_RESULT($enable_upc_runtime_debug)
if test "$enable_upc_runtime_debug" = "yes"; then
  AC_DEFINE(GUPCR_HAVE_DEBUG, 1,
            [Define to 1 if UPC runtime debugging mode is enabled.])
fi
AC_SUBST(enable_upc_runtime_debug)


AC_MSG_CHECKING([for UPC link script support])
AC_ARG_ENABLE(upc-link-script,
[AS_HELP_STRING([--enable-upc-link-script],
                [Enable UPC's use of a custom linker script;
                 this will define the UPC shared section as a
		 no load section on targets where this feature
		 is supported (requires GNU LD)])],
[
  case $enableval in
  yes | no) ;;
  *)
    AC_MSG_ERROR([--enable-upc-link-script accepts only yes or no])
  esac
],
[
  case "$target" in
    # disable linker script for Apple Mac OS X and SunOS
    *-*-darwin*)
      enable_upc_link_script=no
      ;;
    *-*-solaris*)
      enable_upc_link_script=no
      ;;
    *)
      enable_upc_link_script=yes
      ;; 
  esac
])
if test "$enable_upc_link_script" = yes; then
  if test "$with_gnu_ld" = yes; then
    gen_ld_script=${srcdir}/gen-upc-ld-script.pl
    if ! ${LD} --verbose | ${PERL} $gen_ld_script 1>/dev/null ; then
      # if gen-upc-ld-script did not accept the output of the linker
      # then disable UPC link script support.
      AC_MSG_ERROR([selected linker does not support linker scripts])
    fi
  fi
fi
AC_MSG_RESULT($enable_upc_link_script)
if test "$enable_upc_link_script" = yes; then
  AC_DEFINE(HAVE_UPC_LINK_SCRIPT, 1,
    [Define to 1 if UPC link script is supported.])
  LIBUPC_LINK_SCRIPT=1
else
  LIBUPC_LINK_SCRIPT=0
fi
AC_SUBST(LIBUPC_LINK_SCRIPT)

AC_MSG_CHECKING([for UPC runtime tree fanout])
AC_ARG_WITH(upc-runtime-tree-fanout,
[AS_HELP_STRING([--with-upc-runtime-tree-fanout=WIDTH],
		[Specify the maximum number of children in each
		 sub-tree used to implement UPC collectives operations
		 (e. g. upc_barrier)
		 [default=4]])],
[
  # $withval must be a decimal integer
  if ! echo "$withval" | grep '^[[0-9]][[0-9]]*$' >/dev/null 2>&1; then
    AC_MSG_ERROR([$withval is an invalid option to
    --with-upc-runtime-tree-fanout])
  fi
  upc_runtime_tree_fanout="$withval"
],
[
  upc_runtime_tree_fanout=4
])
AC_MSG_RESULT([$upc_runtime_tree_fanout])
AC_DEFINE_UNQUOTED(GUPCR_TREE_FANOUT, [$upc_runtime_tree_fanout],
  [Maximum number of children at each level of a collective operation tree.])

AC_MSG_CHECKING([for UPC backtrace support])
AC_ARG_ENABLE(upc-backtrace,
[AS_HELP_STRING([--enable-upc-backtrace],
		[Enable UPC backtrace;
		 Enable stack frame backtrace report
		 when UPC run-time fatal errors occur
		 or by user request (via signal)])],
[
  case $enableval in
    yes | no) ;;
    *)
      AC_MSG_ERROR([--enable-upc-backtrace accepts only yes or no])
  esac
],
[
  enable_upc_backtrace=yes
])
AC_MSG_RESULT($enable_upc_backtrace)
if test "$enable_upc_backtrace" = yes; then
  AC_DEFINE(HAVE_UPC_BACKTRACE, 1,
    [Define to 1 if UPC backtrace is enabled.])
  LIBUPC_ENABLE_BACKTRACE=1
fi
AC_SUBST(LIBUPC_ENABLE_BACKTRACE)

# Check for user specified GDB
AC_ARG_WITH(upc-backtrace-gdb,
  [AS_HELP_STRING([--with-upc-backtrace-gdb=GDB],
  		  [Specify which GDB to use for UPC backtrace support
  		  [default='gdb']])],
[
  upc_backtrace_gdb=$withval
],
[
  upc_backtrace_gdb=gdb
])
AC_PATH_PROGS(PATH_TO_GDB, $GDB $upc_backtrace_gdb, , /usr/bin:${PATH})
AC_MSG_CHECKING([for UPC backtrace GDB to use])
if test -n "$PATH_TO_GDB"; then
  AC_DEFINE_UNQUOTED(GUPCR_BACKTRACE_GDB, "$PATH_TO_GDB",
    [Define path to preferred GDB for backtrace])
  AC_MSG_RESULT([$PATH_TO_GDB])
  upc_backtrace_gdb_available=yes
else
  upc_backtrace_gdb_available=no
  AC_MSG_RESULT(no)
fi

AC_MSG_CHECKING([for UPC backtrace GDB support])
AC_ARG_ENABLE(upc-backtrace-gdb,
[AS_HELP_STRING([--enable-upc-backtrace-gdb],
		[Enable the use of GDB for UPC stack backtrace
		[default='yes']])],
[
  case "$enableval" in
    yes | no)
      if test "$enableval" = yes &&
         test "$upc_backtrace_gdb_available" = no; then
        AC_MSG_ERROR(
          [Cannot find gdb. Set the correct gdb path with
           --with-upc-backtrace-gdb option or disable gdb backtrace with
           --disable-upc-backtrace-gdb option.])
      fi
      ;;
    *)
      AC_MSG_ERROR([--enable-upc-backtrace-gdb accepts only yes or no])
  esac
],
[
  enable_upc_backtrace_gdb=$upc_backtrace_gdb_available
])
AC_MSG_RESULT([$enable_upc_backtrace_gdb])
if test "$enable_upc_backtrace_gdb" = yes; then
  AC_DEFINE(HAVE_UPC_BACKTRACE_GDB, 1,
    [Define to 1 if UPC backtrace with GDB is enabled.])
fi

AC_MSG_CHECKING([for UPC backtrace signal support])
AC_ARG_ENABLE(upc-backtrace-signal,
[AS_HELP_STRING([--enable-upc-backtrace-signal],
		[Enable signal support for UPC stack backtrace
		[default='yes']])],
[
  case "$enableval" in
  yes | no) ;;
  *)
    AC_MSG_ERROR([--enable-upc-backtrace-signal accepts only yes or no])
  esac
  ],
[
  enable_upc_backtrace_signal=yes
])
AC_MSG_RESULT([$enable_upc_backtrace_signal])
if test "$enable_upc_backtrace_signal" = yes; then
  AC_DEFINE(HAVE_UPC_BACKTRACE_SIGNAL, 1,
    [Define to 1 if UPC backtrace signal is enabled.])

  # Check for user specified signal name/number
  AC_MSG_CHECKING([for UPC backtrace signal name/number])
  AC_ARG_WITH(upc-backtrace-signal,
    [AS_HELP_STRING([--with-upc-backtrace-signal=SIGNAL],
    [Specify the signal to be used for UPC stack backtrace
    [default='SIGUSR1']])],
    [
      upc_backtrace_signal=$withval
    ],
    [
      upc_backtrace_signal=SIGUSR1
    ])
  AC_DEFINE_UNQUOTED(GUPCR_BACKTRACE_SIGNAL, $upc_backtrace_signal,
    [Define to preferred signal for UPC backtrace.])
  AC_MSG_RESULT([$upc_backtrace_signal])
fi

if test "$enable_upc_backtrace" = yes; then
  # Check for "-rdynamic" for UPC backtrace
  case "${host}" in
    *-*-darwin*)
      if test x$build = x$host; then
        export_sym_check="nm${exeext} -g"
      elif test x$host = x$target; then
        export_sym_check="$gcc_cv_nm -g"
      else
        export_sym_check=
      fi
    ;;
    *)
      if test x$build = x$host; then
        export_sym_check="objdump${exeext} -T"
      elif test x$host = x$target; then
        export_sym_check="$gcc_cv_objdump -T"
      else
        export_sym_check=
      fi
    ;;
  esac
  AC_MSG_CHECKING([for -rdynamic])
  if test "x$export_sym_check" != x; then
    echo "int main() {return 0;} int foobar() {return 0;}" > conftest.c
    ${CC} ${CFLAGS} ${LDFLAGS} -rdynamic conftest.c \
      -o conftest > /dev/null 2>&1
    if $export_sym_check conftest | grep foobar > /dev/null; then
      have_rdynamic=yes
    else
      have_rdynamic=no
    fi
  fi
  AC_MSG_RESULT([$have_rdynamic])

  # Check if backtrace can use addr2line
  if test x"$ADDR2LINE" != x""; then
    AC_DEFINE(HAVE_UPC_BACKTRACE_ADDR2LINE, 1,
      [Define to 1 if UPC backtrace with ADDR2LINE is enabled.])
    AC_DEFINE_UNQUOTED(GUPCR_BACKTRACE_ADDR2LINE, "$ADDR2LINE",
      [Define path to preferred addr2line for backtrace])
  fi
fi

# At least for glibc, clock_gettime is in librt.  But don't pull that
# in if it still doesn't give us the function we want.
AC_CHECK_FUNCS([clock_gettime])
if test $ac_cv_func_clock_gettime = no; then
  AC_CHECK_LIB(rt, clock_gettime,[ac_cv_func_clock_gettime="yes"])
  if test $ac_cv_func_clock_gettime = yes; then
    LIBS="-lrt $LIBS"
    AC_DEFINE(HAVE_CLOCK_GETTIME, 1,
              [Define to 1 if you have the `clock_gettime' function.])
  fi
fi 

if test "$upc_runtime_model" = "smp"; then

  AC_CHECK_FUNC(sched_getaffinity,[affinity_available=yes],[affinity_available=no])
  AC_MSG_CHECKING(for Linux scheduling affinity)
  AC_ARG_ENABLE(upc-affinity,
  [AS_HELP_STRING([--enable-upc-affinity],
                  [enable UPC runtime scheduling affinity [default='yes']])],
  [
    case $enableval in
    yes)
      if test "x$affinity_available" = "xno"; then
        AC_MSG_ERROR([Scheduling affinity enabled but not supported - use --disable-upc-affinity])
      fi
      upc_affinity=yes ;;
    no)
      upc_affinity=no ;;
    *)
      AC_MSG_ERROR([--enable-upc-affinity accepts only yes or no])
    esac
  ],
  [
    if test "x$affinity_available" = "xyes"; then
      upc_affinity=yes
    else
      upc_affinity=no
    fi
  ])
  AC_MSG_RESULT([$upc_affinity])
  if test "$upc_affinity" = yes; then
    LIBUPC_ENABLE_AFFINITY=1
  fi
  AC_SUBST(LIBUPC_ENABLE_AFFINITY)

  AC_CHECK_HEADERS([numa.h],[numa_available=yes],[numa_available=no])
  AC_MSG_CHECKING([if NUMA is available])
  AC_ARG_ENABLE(upc-numa,
  [AS_HELP_STRING([--enable-upc-numa],
                  [enable UPC runtime NUMA support [default='yes']])],
  [
    case $enableval in
    yes)
      if test "x$numa_available" = "xno"; then
        AC_MSG_ERROR([NUMA enabled but not supported - use --disable-upc-numa])
      fi
      upc_numa=yes ;;
    no)
      upc_numa=no ;;
    *)      AC_MSG_ERROR([--enable-upc-numa accepts only yes or no])
    esac
  ],
  [
    if test "x$numa_available" = "xyes"; then
      upc_numa=yes
    else
      upc_numa=no
    fi
  ])
  AC_MSG_RESULT([$upc_numa])
  if test "$upc_numa" = yes; then
    LIBUPC_ENABLE_NUMA=1
  fi
  AC_SUBST(LIBUPC_ENABLE_NUMA)

  AC_MSG_CHECKING([for UPC GUM debugging support])
  AC_ARG_ENABLE(upc-gum-debug,
  [AS_HELP_STRING([--enable-upc-gum-debug],
                  [enable UPC's source level debugging with GDB and GUM
                   support (requires UPC aware GDB)])],
  [
    case $enableval in
      yes | no) ;;
      *)
        AC_MSG_ERROR([--enable-upc-gum-debug accepts only yes or no])
    esac
    upc_gum_debug=$enableval
  ],
  [
    upc_gum_debug=no
  ])
  AC_MSG_RESULT([$upc_gum_debug])
  if test "x${upc_gum_debug}" = "xyes"; then
    AC_DEFINE_UNQUOTED(GUPCR_HAVE_GUM_DEBUG,[1],
    [Define if UPC GUM debug server is supported.])
  fi
  # Use the generic (reference implementation)
  use_generic_collectives="yes"

fi #### End: SMP runtime configuration

if test "$upc_runtime_model" = "portals4"; then

  #### Portals4 runtime library configuration ####

  # Use the generic (reference implementation)
  use_generic_collectives="no"

  AC_CHECK_HEADERS([portals4.h],[have_p4=yes],[have_p4=no])
  AC_ARG_WITH(portals4,
  [AS_HELP_STRING([--with-portals4=PATH],
                  [Specify prefix directory for installed portals4 package])],
  [
    if test -f "$withval/include/portals4.h"; then
      AC_DEFINE_UNQUOTED(LIBUPC_PORTALS4, "$withval", [Portals4 library path] )
      AC_SUBST(LIBUPC_PORTALS4,$withval)
    else
      AC_MSG_ERROR([--with-portals4 specifies wrong prefix for Portals4 package.])
    fi
  ],
  [
    if test "$have_p4" = "no"; then
      AC_MSG_ERROR([Portals4 not found - use --with-portals4 to specify one.])
    fi
  ])

  AC_MSG_CHECKING([for UPC runtime Portals4 PTE base index])
  AC_ARG_WITH(upc-runtime-pte-base,
  [AS_HELP_STRING([--with-upc-runtime-pte-base=BASE],
		  [Specify the base index of the first Portals4 PTE used
		   by the UPC runtime [default=16]])],
  [
    # $withval must be a decimal integer
    if ! echo "$withval" | grep '^[[0-9]][[0-9]]*$' >/dev/null 2>&1; then
      AC_MSG_ERROR([$withval is an invalid option to --with-upc-runtime-pte-base])
    fi
    upc_runtime_pte_base="$withval"
  ],
  [
    upc_runtime_pte_base=16
  ])
  AC_MSG_RESULT([$upc_runtime_pte_base])
  AC_DEFINE_UNQUOTED(GUPCR_PTE_BASE,[$upc_runtime_pte_base],
    [Portals4 PTE base index.])
fi

if test "$upc_runtime_model" = "fabric"; then

  #### Fabric runtime library configuration ####

  # Use the generic (reference implementation)
  use_generic_collectives="no"

  AC_CHECK_HEADERS([fabric.h],[have_fabric=yes],[have_fabric=no])             
  AC_ARG_WITH(fabric,
  [AS_HELP_STRING([--with-fabric=PATH],
                  [Specify prefix directory for installed libfabric package])],
  [
    if test -f "$withval/include/rdma/fabric.h"; then
      AC_DEFINE_UNQUOTED(LIBUPC_FABRIC, "$withval", [Libfabric library path] )
      AC_SUBST(LIBUPC_FABRIC,$withval)
    else
      AC_MSG_ERROR([--with-fabric specifies wrong prefix for Libfabric package.])
    fi
  ],
  [
    if test "$have_fabric" = "no"; then
      AC_MSG_ERROR([Libfabric not found - use --with-fabric to specify one.])
    fi
  ])

  AC_MSG_CHECKING([for libfabric network device])
  AC_ARG_WITH(fabric-device,
  [AS_HELP_STRING([--with-fabric-device=DEVICE],
                  [Specify network device for fabric interface [default ib0]])],
  [
    fabric_device="$withval"
  ],
  [
    fabric_device="ib0"
  ])
  AC_MSG_RESULT($fabric_device)
  AC_DEFINE_UNQUOTED(GUPCR_LIBFABRIC_DEVICE, "$fabric_device", [Libfabric device] )
  AC_SUBST(GUPCR_LIBFABRIC_DEVICE,$fabric_device)

  AC_MSG_CHECKING([for libfabric provider])
  AC_ARG_WITH(fabric-provider,
  [AS_HELP_STRING([--with-fabric-provider=PROVIDER],
                  [Specify libfabric provider [default sockets]])],
  [
    fabric_provider="$withval"
  ],
  [
    fabric_provider="sockets"
  ])
  AC_MSG_RESULT($fabric_provider)
  AC_DEFINE_UNQUOTED(GUPCR_LIBFABRIC_PROVIDER, "$fabric_provider", [Libfabric provider] )
  AC_SUBST(GUPCR_LIBFABRIC_PROVIDER,$fabric_provider)

  AC_MSG_CHECKING([for libfabric shared context usage])
  AC_ARG_ENABLE(fabric-shared-ctx,
  [AS_HELP_STRING([--enable-fabric-shared-ctx],
		  [Enable UPC runtime support for shared
		   TX/RX context [default=no]])],
  [
    case $enableval in
    yes | no) ;;
    *)
      AC_MSG_ERROR([--enable-fabric-shared-ctx accepts only yes or no.])
    esac
    enable_fabric_shared_ctx=$enableval
  ],
  [
    enable_fabric_shared_ctx=no
  ])
  AC_MSG_RESULT($enable_fabric_shared_ctx)
  if test "$enable_fabric_shared_ctx" = "yes"; then
    AC_DEFINE(GUPCR_LIBFABRIC_SHARED_CTX, 1,
      [Define to 1 if UPC runtime will use shared TX/RX context.])
  fi

fi #### End: Fabric configuration

if test "$upc_runtime_model" = "portals4" ||
   test "$upc_runtime_model" = "fabric"; then

  #### Infiniband based runtime library configuration ####

  AC_MSG_CHECKING([for UPC runtime bounce buffer size])
  AC_ARG_WITH(upc-runtime-bounce-buffer-size,
  [AS_HELP_STRING([--with-upc-runtime-bounce-buffer-size=SIZE],
		  [Specify the size (in bytes) of the bounce buffer
		   that is used by the UPC runtime to buffer
		   network data [default=256K]])],
  [
    # $withval must be a decimal integer
    if ! echo "$withval" | egrep -i '^[[0-9]]+[[kmg]]?$' >/dev/null 2>&1; then
      AC_MSG_ERROR([$withval is an invalid option to
			--with-upc-runtime-bounce-buffer-size])
    fi
    bb_size="$withval"
  ],
  [
    bb_size="256K"
  ])
  bb_size_expr=`echo "$bb_size" | \
    sed -e 's/[[gG]]$/*1024M/;s/[[mM]]$/*1024K/;s/[[kK]]$/*1024/;s/\\*/ \\\\* /g'`
  upc_runtime_bounce_buffer_size=`eval expr $bb_size_expr`
  AC_MSG_RESULT([$upc_runtime_bounce_buffer_size])
  AC_DEFINE_UNQUOTED(GUPCR_BOUNCE_BUFFER_SIZE,[$upc_runtime_bounce_buffer_size],
    [Size of get/put bounce buffer])

  AC_MSG_CHECKING([for UPC maximum number of outstanding remote puts])
  AC_ARG_WITH(upc-max-outstanding-puts,
  [AS_HELP_STRING([--with-upc-max-outstanding-puts=SIZE],
		  [Specify the maximum number of outstanding
		   remote put requests [default=256]])],
  [
    # $withval must be a decimal integer
    if ! echo "$withval" | grep '^[[0-9]]*$' >/dev/null 2>&1; then
      AC_MSG_ERROR([$withval is an invalid option to
			--with-upc-max-outstanding-puts])
    fi
    upc_max_puts_count="$withval"
  ],
  [
    upc_max_puts_count="256"
  ])
  AC_MSG_RESULT([$upc_max_puts_count])
  AC_DEFINE_UNQUOTED(GUPCR_MAX_OUTSTANDING_PUTS,[$upc_max_puts_count],
    [Maximum number of outstanding remote puts])

  AC_MSG_CHECKING([for UPC runtime triggered operation support])
  AC_ARG_ENABLE(upc-runtime-triggered-ops,
  [AS_HELP_STRING([--enable-upc-runtime-triggered-ops],
		  [Enable UPC runtime support for triggered
		   operations [default=yes]])],
  [
    case $enableval in
    yes | no) ;;
    *)
      AC_MSG_ERROR([--enable-upc-runtime-triggered-ops accepts only yes or no.])
    esac
  ],
  [
    enable_upc_runtime_triggered_ops=yes
  ])
  AC_MSG_RESULT($enable_upc_runtime_triggered_ops)
  if test "$enable_upc_runtime_triggered_ops" = "yes"; then
    AC_DEFINE(GUPCR_USE_TRIGGERED_OPS, 1,
      [Define to 1 if UPC runtime will use triggered operations.])
  fi

  # Node local memory optimization

  AC_MSG_CHECKING([for UPC runtime node local memory access])
  AC_ARG_ENABLE(upc-node-local-mem,
  [AS_HELP_STRING([--enable-upc-node-local-mem],
		  [Enable UPC runtime support for accessing shared
		   memory of the node local threads [default=yes]])],
  [
    case $enableval in
    yes | no) ;;
    *)
      AC_MSG_ERROR([--enable-upc-node-local-mem accepts only yes or no.])
    esac
  ],
  [
    enable_upc_node_local_mem=yes
  ])
  AC_MSG_RESULT($enable_upc_node_local_mem)
  if test "$enable_upc_node_local_mem" = "yes"; then
    AC_DEFINE(GUPCR_NODE_LOCAL_MEM, 1,
      [Define to 1 if UPC runtime will use node local memory accesses.])
  fi

  if test "$enable_upc_node_local_mem" = "yes"; then
    # START - check for shared memory type
    # Check for Posix based shm
    AC_MSG_CHECKING([for UPC node local memory type])
    AC_ARG_WITH(upc-node-local-mem,
    [AS_HELP_STRING([--with-upc-node-local-mem=SHMEM],
		    [Specify type of shared memory used for node local
		     memory accesses (posix or mmap)
		    [default=posix]])],
    [
      case $withval in
        posix | mmap)
	  upc_node_local=$withval
	  ;;
        *)
          AC_MSG_ERROR([--with-upc-node-local-mem accepts only posix or mmap.])
      esac
    ],
    [
      upc_node_local=posix
    ])
    AC_MSG_RESULT($upc_node_local);

    # Check if POSIX shm is available
    if test "$upc_node_local" = "posix"; then
      # Check if it is part of librt
      have_shm_open=no
      AC_CHECK_LIB(rt, shm_open,
         [LIBS="-lrt $LIBS"
 	  have_shm_open=yes
          AC_DEFINE(HAVE_SHM_OPEN, 1,
            [Define to 1 if you have the 'shm_open' function.])])
      if test "$have_shm_open" = "no"; then
        # Must probe for Posix shmem
        AC_MSG_CHECKING([for the shm_open])
        AC_LINK_IFELSE(
          AC_LANG_SOURCE(
          [[#include <sys/types.h>
          #include <sys/stat.h>
          #include <sys/mman.h>
          #include <unistd.h>
          #include <fcntl.h>
          #include <stdlib.h>
          int size = 4096;
          int fd;
          void * addr;
          int main () {
          fd = shm_open("shm-config-test", O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
          (void)shm_unlink("shm-config-test");
          if (fd < 0) return 1;
          if (ftruncate(fd, size) < 0) return 1;
          addr = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
          (void)close(fd);
          if (!addr || (addr == MAP_FAILED)) return 1;
          (void)munmap(addr, size);
          return 0;}]]),
            [AC_MSG_RESULT(yes); have_shm_open=yes ],
	    [AC_MSG_RESULT(no); have_shm_open=no ],
	    [AC_MSG_RESULT(no); have_shm_open=no ])
      fi

      if test "$have_shm_open" = "yes"; then
        AC_DEFINE(GUPCR_NODE_LOCAL_MEM_POSIX, 1,
          [Define to 1 if UPC node local access uses Posix shared memory.])
      else
        AC_MSG_ERROR([Posix shared memory is not available])
      fi
    else
      # mmap is always available
      AC_DEFINE(GUPCR_NODE_LOCAL_MEM_MMAP, 1,
        [Define to 1 if UPC node local access uses mmap-ed file.])
    fi
    # END - check for shared memory type
  fi

  # Check for job launcher
  AC_CHECK_HEADER([slurm/slurm.h],[have_slurm=yes],[have_slurm=no])
  AC_MSG_CHECKING([for UPC runtime job launcher])
  AC_ARG_WITH(upc-job-launcher,
  [AS_HELP_STRING([--with-upc-job-launcher],
		  [Select UPC job launcher
		   [default=slurm]])],
  [
    case $withval in
    slurm | yod)
      upc_job_launcher=$withval
      ;;
    *)
      AC_MSG_ERROR([--with-upc-job-launcher accepts only slurm or yod.])
    esac
  ],
  [
    if test "$have_slurm" = "yes"; then
      upc_job_launcher=slurm
    else
      upc_job_launcher=yod
    fi
  ])
  AC_MSG_RESULT($upc_job_launcher)

  if test "$upc_job_launcher" = "slurm"; then
    if test "$have_slurm" = "yes"; then
      AC_DEFINE(GUPCR_JOB_LAUNCHER_SLURM, 1,
      [Use SLURM for UPC job launcher])
    else
      AC_MSG_ERROR([--with-upc-job-launcher specified slurm,
                    but slurm is not available.])
    fi
  else
    AC_DEFINE(GUPCR_JOB_LAUNCHER_YOD, 1,
    [Use YOD for UPC Portals4 job launcher])
  fi

  AC_DEFINE(GUPCR_GLOBAL_EXIT_TIMEOUT, [2],
    [upc_global_exit() timeout in seconds.])
  AC_SUBST(GUPCR_GLOBAL_EXIT_TIMEOUT, 2)

fi #### End: Infiniband based runtime configuration

# Check for target memory page size
AC_MSG_CHECKING([for memory page size])
AC_ARG_WITH(upc-memory-page-size,
[AS_HELP_STRING([--with-upc-memory-page-size],
		[Select target memory page size
		[default=4096]])],
[
  # $withval must be a decimal integer
  if ! echo "$withval" | grep '^[[0-9]][[0-9]]*$' >/dev/null 2>&1; then
    AC_MSG_ERROR([$withval is an invalid option to --with-upc-memory-page-size])
  fi
  memory_page_size="$withval"
],
[
  memory_page_size="4096"
])
AC_MSG_RESULT($memory_page_size)
AC_DEFINE_UNQUOTED(GUPCR_MEMORY_PAGE_SIZE, [$memory_page_size],
  [Target system memory page size.])

AC_MSG_CHECKING([for UPC runtime maximum number of locks held per thread])
AC_ARG_WITH(upc-runtime-max-locks,
[AS_HELP_STRING([--with-upc-runtime-max-locks=MAX_LOCKS],
                [specify the maximum number of locks that can be held by
                 a single UPC thread [default=1024]])],
[
  # $withval must be a decimal integer
  if ! echo "$withval" | grep '^[[0-9]][[0-9]]*$' >/dev/null 2>&1; then
    AC_MSG_ERROR([$withval is an invalid option to
    --with-upc-runtime-max-locks])
  fi
  upc_runtime_max_locks="$withval"
],
[
  upc_runtime_max_locks=1024
])
AC_MSG_RESULT([$upc_runtime_max_locks])
AC_DEFINE_UNQUOTED(GUPCR_MAX_LOCKS,[$upc_runtime_max_locks],
  [Maximum number of locks held per thread])

dnl Define ARG_UNUSED macro
AH_BOTTOM([
#ifndef ARG_UNUSED
# define ARG_UNUSED(NAME) NAME __attribute__ ((__unused__))
#endif
])

dnl Configure configuration files
AC_CONFIG_HEADERS(config.h)

dnl Configure a common Makefile
AC_CONFIG_FILES(Makefile.rules)
AC_CONFIG_FILES(Makefile.optional)

dnl Configure project makefiles
AC_CONFIG_MAKEFILE(Makefile)

dnl This must be last
AC_OUTPUT
