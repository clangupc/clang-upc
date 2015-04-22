/* This generated file is for internal use. Do not include it from headers. */

#ifdef CONFIG_H
#error config.h can only be included once
#else
#define CONFIG_H

/* Bug report URL. */
#define BUG_REPORT_URL "${BUG_REPORT_URL}"

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

/* Libfabric support */
#cmakedefine LIBUPC_FABRIC "${LIBUPC_FABRIC}"

/* SLURM job launcher support */                                                    
#cmakedefine LIBUPC_JOB_LAUNCHER_SLURM ${LIBUPC_JOB_LAUNCHER_SLURM}

/* PMI job manager support */                                                    
#cmakedefine LIBUPC_JOB_PMI_LIBRARY_DIR "${LIBUPC_JOB_PMI_LIBRARY_DIR}"

/* Define if we have libxml2 */
#cmakedefine CLANG_HAVE_LIBXML ${CLANG_HAVE_LIBXML}

/* The LLVM product name and version */
#define BACKEND_PACKAGE_STRING "${BACKEND_PACKAGE_STRING}"

#endif
