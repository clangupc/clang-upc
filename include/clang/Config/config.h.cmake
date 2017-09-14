/* This generated file is for internal use. Do not include it from headers. */

#ifdef CLANG_CONFIG_H
#error config.h can only be included once
#else
#define CLANG_CONFIG_H

/* Bug report URL. */
#define BUG_REPORT_URL "${BUG_REPORT_URL}"

/* Default linker to use. */
#define CLANG_DEFAULT_LINKER "${CLANG_DEFAULT_LINKER}"

/* Default C++ stdlib to use. */
#define CLANG_DEFAULT_CXX_STDLIB "${CLANG_DEFAULT_CXX_STDLIB}"

/* Default runtime library to use. */
#define CLANG_DEFAULT_RTLIB "${CLANG_DEFAULT_RTLIB}"

/* Default OpenMP runtime used by -fopenmp. */
#define CLANG_DEFAULT_OPENMP_RUNTIME "${CLANG_DEFAULT_OPENMP_RUNTIME}"

/* Multilib suffix for libdir. */
#define CLANG_LIBDIR_SUFFIX "${CLANG_LIBDIR_SUFFIX}"

/* Relative directory for resource files */
#define CLANG_RESOURCE_DIR "${CLANG_RESOURCE_DIR}"

/* Directories clang will search for headers */
#define C_INCLUDE_DIRS "${C_INCLUDE_DIRS}"

/* Default <path> to all compiler invocations for --sysroot=<path>. */
#define DEFAULT_SYSROOT "${DEFAULT_SYSROOT}"

/* Directory where gcc is installed. */
#define GCC_INSTALL_PREFIX "${GCC_INSTALL_PREFIX}"

/* UPC shared pointer representation. */
#cmakedefine UPC_PTS "${UPC_PTS}"

/* the number of bits in each field. */
#cmakedefine UPC_PACKED_BITS "${UPC_PACKED_BITS}"

/* the field order */
#cmakedefine UPC_PTS_VADDR_ORDER "${UPC_PTS_VADDR_ORDER}"

/* UPC Remote Pointer thread field size */
#cmakedefine UPC_IR_RP_THREAD ${UPC_IR_RP_THREAD}

/* UPC Remote Pointer address field size */
#cmakedefine UPC_IR_RP_ADDR ${UPC_IR_RP_ADDR}

/* UPC Remote Pointer address space */
#cmakedefine UPC_IR_RP_ADDRSPACE ${UPC_IR_RP_ADDRSPACE}

/* UPC runtime NUMA enable. */
#cmakedefine LIBUPC_ENABLE_NUMA 1

/* UPC link script enabled. */
#cmakedefine LIBUPC_LINK_SCRIPT 1

/* Enable UPC backtrace */
#cmakedefine LIBUPC_ENABLE_BACKTRACE 1

/* Portals4 support */
#cmakedefine LIBUPC_PORTALS4 "${LIBUPC_PORTALS4}"

/* Portals4 SLURM support */                                                    
#cmakedefine LIBUPC_PORTALS4_SLURM ${LIBUPC_PORTALS4_SLURM}

/* UPC enable OMP checks */
#cmakedefine LIBUPC_ENABLE_OMP_CHECKS ${LIBUPC_ENABLE_OMP_CHECKS}

/* Define if we have libxml2 */
#cmakedefine CLANG_HAVE_LIBXML ${CLANG_HAVE_LIBXML}

/* Define if we have sys/resource.h (rlimits) */
#cmakedefine CLANG_HAVE_RLIMITS ${CLANG_HAVE_RLIMITS}

/* The LLVM product name and version */
#define BACKEND_PACKAGE_STRING "${BACKEND_PACKAGE_STRING}"

/* Linker version detected at compile time. */
#cmakedefine HOST_LINK_VERSION "${HOST_LINK_VERSION}"

/* pass --build-id to ld */
#cmakedefine ENABLE_LINKER_BUILD_ID

/* enable x86 relax relocations by default */
#cmakedefine01 ENABLE_X86_RELAX_RELOCATIONS

#endif
