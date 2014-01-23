# See www/CMake.html for instructions on how to build libupc with CMake.

#===============================================================================
# Setup Project
#===============================================================================

project(libupc C)
cmake_minimum_required(VERSION 2.8)

set(PACKAGE_NAME libupc)
set(PACKAGE_VERSION trunk-svn)
set(PACKAGE_STRING "${PACKAGE_NAME} ${PACKAGE_VERSION}")

#===============================================================================
# Setup CMake Options
#===============================================================================

# Define options.
option(LIBUPC_ENABLE_SHARED "Build libupc as a shared library. (Broken)" OFF)

#===============================================================================
# Configure System
#===============================================================================

# Get triples.
#include(GetTriple)
#get_host_triple(LIBUPC_HOST_TRIPLE
#  LIBUPC_HOST_ARCH
#  LIBUPC_HOST_VENDOR
#  LIBUPC_HOST_OS
#  )
#set(LIBUPC_HOST_TRIPLE ${LIBUPC_HOST_TRIPLE} CACHE STRING "Host triple.")
#get_target_triple(LIBUPC_TARGET_TRIPLE
#  LIBUPC_TARGET_ARCH
#  LIBUPC_TARGET_VENDOR
#  LIBUPC_TARGET_OS
#  )
#set(LIBUPC_TARGET_TRIPLE ${LIBCXX_TARGET_TRIPLE} CACHE STRING "Target triple.")

# Configure compiler.
#include(config-ix)

#try_run(LIBUPC_AFFINITY_COMPILE LIBUPC_AFFINITY_RUN conf_affinity.c)

# Configure libupc

set(LIBUPC_RUNTIME_MODEL SMP CACHE STRING "specify the runtime implementation model for UPC, where MODEL may be: 'SMP' (Symmetric Multiprocessing) or 'Portals4'")

set(LIBUPC_ENABLE_LINK_SCRIPT TRUE CACHE STRING "enable UPC's use of a custom linker script; this will define the UPC shared section as a no load section on targets where this feature is supported (requires GNU LD)")

set(LIBUPC_ENABLE_BACKTRACE TRUE CACHE STRING "enable UPC backtrace; enable stack frame backtrace report when UPC run-time fatal errors occur or by user request (via signal)")
set(HAVE_UPC_BACKTRACE ${LIBUPC_ENABLE_BACKTRACE})

set(LIBUPC_ENABLE_BACKTRACE_ADDR2LINE TRUE CACHE STRING "enable the use of addr2line for UPC stack backtrace")
set(HAVE_UPC_BACKTRACE_ADDR2LINE ${LIBUPC_ENABLE_BACKTRACE_ADDR2LINE})
if(LIBUPC_ENABLE_BACKTRACE_ADDR2LINE)
  find_program(LIBUPC_BACKTRACE_ADDR2LINE addr2line)
  set(GUPCR_BACKTRACE_ADDR2LINE ${LIBUPC_BACKTRACE_ADDR2LINE})
endif()

set(LIBUPC_ENABLE_BACKTRACE_GDB TRUE CACHE STRING "enable the use of GDB for UPC stack backtrace")
set(HAVE_UPC_BACKTRACE_GDB ${LIBUPC_ENABLE_BACKTRACE_GDB})
if(LIBUPC_ENABLE_BACKTRACE_GDB)
  find_program(LIBUPC_BACKTRACE_GDB gdb)
  set(GUPCR_BACKTRACE_GDB ${LIBUPC_BACKTRACE_GDB})
endif()

set(LIBUPC_ENABLE_BACKTRACE_SIGNAL TRUE CACHE STRING "enable signal support for UPC stack backtrace")
set(HAVE_UPC_BACKTRACE_SIGNAL ${LIBUPC_ENABLE_BACKTRACE_SIGNAL})
if(LIBUPC_ENABLE_BACKTRACE_SIGNAL)
  set(LIBUPC_BACKTRACE_SIGNAL SIGUSR1 CACHE STRING "the signal to be used for UPC stack backtrace")
  set(GUPCR_BACKTRACE_SIGNAL ${LIBUPC_BACKTRACE_SIGNAL})
endif()

# GUPC_BOUNCE_BUFFER_SIZE
set(LIBUPC_BOUNCE_BUFFER_SIZE 256K CACHE STRING "the size (in bytes) of the bounce buffer that is used by the UPC runtime to buffer network data")
if(NOT LIBUPC_BOUNCE_BUFFER_SIZE MATCHES "^([0-9]+[gGmMkK]?)$")
  message(FATAL_ERROR "Invalid LIBUPC_BOUNCE_BUFFER_SIZE")
endif()
set(tmp ${LIBUPC_BOUNCE_BUFFER_SIZE})
string(REGEX REPLACE [gG] "*1024M" tmp ${tmp})
string(REGEX REPLACE [mM] "*1024K" tmp ${tmp})
string(REGEX REPLACE [kK] "*1024" tmp ${tmp})
math(EXPR GUPCR_BOUNCE_BUFFER_SIZE ${tmp})

set(LIBUPC_GLOBAL_EXIT_TIMEOUT 2 CACHE STRING "upc_global_exit() timeout in seconds.")
set(GUPCR_GLOBAL_EXIT_TIMEOUT ${LIBUPC_GLOBAL_EXIT_TIMEOUT})

set(LIBUPC_ENABLE_RUNTIME_CHECKS FALSE CACHE BOOL "enable internal UPC runtime checks that validate arguments, and check for inconsistent runtime state.")
set(GUPCR_HAVE_CHECKS ${LIBUPC_ENABLE_RUNTIME_CHECKS})

set(LIBUPC_ENABLE_RUNTIME_DEBUG FALSE CACHE BOOL "enable UPC runtime debugging mode")
set(GUPCR_HAVE_DEBUG ${LIBUPC_ENABLE_RUNTIME_DEBUG})

set(LIBUPC_ENABLE_GUM_DEBUG FALSE CACHE BOOL "enable UPC's source level debugging with GDB and GUM support (requires UPC aware GDB)])")
set(GUPCR_HAVE_GUM_DEBUG ${LIBUPC_ENABLE_GUM_DEBUG})

set(LIBUPC_ENABLE_RUNTIME_STATS FALSE CACHE BOOL "enable internal UPC runtime statistics collection support; these statistics count the number of various significant internal operations, and dump those counts into a per-process statistics file.")
set(GUPCR_HAVE_STATS ${LIBUPC_ENABLE_RUNTIME_STATS})

set(LIBUPC_ENABLE_RUNTIME_TRACE FALSE CACHE BOOL "enable internal UPC runtime trace collection support; a runtime trace is a time stamped log that records various significant internal events; this trace is written to a per-process log file.")
set(GUPCR_HAVE_TRACE ${LIBUPC_ENABLE_RUNTIME_TRACE})

set(LIBUPC_RUNTIME_MAX_LOCKS 1024 CACHE STRING "The maximum number of locks that can be held by a single UPC thread")
set(GUPCR_MAX_LOCKS ${LIBUPC_RUNTIME_MAX_LOCKS})

set(LIBUPC_NODE_LOCAL_MEM posix CACHE STRING "specify type of shared memory used for node local memory accesses. (posix, mmap, or none)")
if(LIBUPC_NODE_LOCAL_MEM STREQUAL "posix")
  set(CMAKE_REQUIRED_LIBRARIES rt)
  check_cxx_source_compiles("
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
int size = 4096;
int fd;

void * addr;

int main() {
  fd = shm_open(\"shm-config-test\", O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
  (void)shm_unlink(\"shm-config-test\");
  if (fd < 0) return 1;
  if (ftruncate(fd, size) < 0) return 1;
  addr = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
  (void)close(fd);
  if (!addr || (addr == MAP_FAILED)) return 1;
  (void)munmap(addr, size);
  return 0;
}
"
    HAVE_SHM_OPEN)
  set(CMAKE_REQUIRED_LIBRARIES)

  if(HAVE_SHM_OPEN)
    set(GUPCR_NODE_LOCAL_MEM 1)
    set(GUPCR_NODE_LOCAL_MEM_POSIX 1)
  else()
    message(warning "shm_open not found.  LIBUPC_NODE_LOCAL_MEM=posix ignored")
  endif()
elseif(LIBUPC_NODE_LOCAL_MEM STREQUAL "mmap")
  set(GUPCR_NODE_LOCAL_MEM 1)
  set(GUPCR_NODE_LOCAL_MEM_MMAP 1)
elseif(LIBUPC_NODE_LOCAL_MEM STREQUAL "none")
  # okay
else()
  message(fatal_error "invalid value of LIBUPC_LOCAL_MEM")
endif()

set(LIBUPC_RUNTIME_PTE_BASE 16 CACHE STRING "specify the base index of the first Portals4 PTE used by the UPC runtime")
set(GUPCR_PTE_BASE ${LIBUPC_RUNTIME_PTE_BASE})

set(LIBUPC_RUNTIME_ENABLE_TRIGGERED_OPS TRUE CACHE STRING "enable UPC runtime support for Portals4 triggered operations.")
set(GUPCR_USE_PORTALS4_TRIGGERED_OPS ${LIBUPC_RUNTIME_ENABLE_TRIGGERED_OPS})


set(LIBUPC_RUNTIME_TREE_FANOUT 4 CACHE STRING "The maximum number of children in each sub-tree used to implement UPC collectives operations (e. g. upc_barrier)")
set(GUPCR_TREE_FANOUT ${LIBUPC_RUNTIME_TREE_FANOUT})

set(CMAKE_REQUIRED_LIBRARIES rt)
check_function_exists(clock_gettime HAVE_CLOCK_GETTIME)
set(CMAKE_REQUIRED_LIBRARIES)

check_include_file(execinfo.h HAVE_EXECINFO_H)
check_include_file(limits.h HAVE_LIMITS_H)
check_include_file(sys/time.h HAVE_SYS_TIME_H)

# CMake 2.8.4 doesn't accept check_c_source_compiles
check_cxx_source_compiles("
typedef unsigned int uint32  __attribute__ ((mode (SI)));
uint32 i;
int main() { return __sync_fetch_and_add (&i, 1); }
"
HAVE_SYNC_FETCH_AND_ADD_4)

check_cxx_source_compiles("
typedef unsigned int uint64  __attribute__ ((mode (DI)));
uint64 i;
int main() { return __sync_fetch_and_add (&i, 1); }
"
HAVE_SYNC_FETCH_AND_ADD_8)

set(CMAKE_REQUIRED_LIBRARIES)

set(HAVE_UPC_LINK_SCRIPT ${LIBUPC_ENABLE_LINK_SCRIPT})

set(TIME_WITH_SYS_TIME TRUE)

set(CMAKE_C_COMPILER ${LLVM_TOOLS_BINARY_DIR}/clang)

#===============================================================================
# Setup Compiler Flags
#===============================================================================

set(LIBUPC_WARNING_FLAGS "-Wno-gnu -Wno-language-extension-token")

# Get required flags.

macro(append_if list condition var)
  if (${condition})
    list(APPEND ${list} ${var})
  endif()
endmacro()

# Get feature flags.

# This is the _ONLY_ place where add_definitions is called.
add_definitions(
 ${LIBUPC_REQUIRED_FLAGS}
 ${LIBUPC_WARNING_FLAGS}
 ${LIBUPC_FEATURE_FLAGS}
 )

set(LIBUPC_CONFIGURATIONS "p;s;p-l;s-l" CACHE STRING "UPC Pointer Representation e.g. p-f-20-10-34;s-l;p-l")

set(all_configs ${LIBUPC_CONFIGURATIONS})
if(all_configs)
  set(CONFIGURATIONS)
  foreach(conf ${all_configs})
    set(okay)
    string(REGEX MATCH "^(-([psfl]|[0-9]+))*$" okay "-${conf}")
    if(okay)
      list(APPEND CONFIGURATIONS ${conf})
    else()
      message(WARNING "Invalid library specification ${conf}.")
    endif()
  endforeach()
else()
  set(CONFIGURATIONS p)
endif()

configure_file(config.h.cmake config.h)

#===============================================================================
# Setup Source Code
#===============================================================================

include_directories(include collectives)

if(LIBUPC_RUNTIME_MODEL STREQUAL SMP)

set(LIBUPC_SOURCES

  smp/upc_access.c
  smp/upc_accessg.c
  smp/upc_addr.c
  # smp/upc_affinity.c
  smp/upc_affinity_stub.c
  smp/upc_allocg.upc
  smp/upc_alloc.upc
  smp/upc_atomic.upc
  smp/upc_barrier.upc
  smp/upc_castable.upc
  smp/upc_debug.c
  smp/upc_gasp.c
  smp/upc_gum.c
  smp/upc_libg.c
  # smp/upc_lock_sup.c
  smp/upc_lock.upc
  smp/upc_main.c
  smp/upc_mem.c
  smp/upc_nb.upc
  # smp/upc_numa.c
  smp/upc_numa_stub.c
  smp/upc_pgm_info.c
  smp/upc_pupc.c
  smp/upc_sysdep.c
  smp/upc_tick.c
  smp/upc_vm.c
)

if(LIBUPC_ENABLE_BACKTRACE)
  list(APPEND LIBUPC_SOURCES smp/upc_backtrace.c)
endif()

list(APPEND LIBUPC_SOURCES
  collectives/upc_coll_broadcast.upc
  collectives/upc_coll_err.upc
  collectives/upc_coll_exchange.upc
  collectives/upc_coll_gather.upc
  collectives/upc_coll_gather_all.upc
  collectives/upc_coll_init.upc
  collectives/upc_coll_permute.upc
  collectives/upc_coll_prefix_reduce.upc
  collectives/upc_coll_reduce.upc
  collectives/upc_coll_scatter.upc
  collectives/upc_coll_sort.upc
)

elseif(LIBUPC_RUNTIME_MODEL STREQUAL Portals4)
  set(LIBUPC_SOURCES
    portals4/gupcr_access.c
    portals4/gupcr_addr.c
    portals4/gupcr_alloc.upc
    portals4/gupcr_barrier.c
    portals4/gupcr_broadcast.c
    portals4/gupcr_clock.c
    portals4/gupcr_coll_broadcast.upc
    portals4/gupcr_coll_init.upc
    portals4/gupcr_coll_reduce.upc
    portals4/gupcr_coll_sup.c
    portals4/gupcr_env.c
    portals4/gupcr_gmem.c
    portals4/gupcr_lock_sup.c
    portals4/gupcr_lock.upc
    portals4/gupcr_main.c
    portals4/gupcr_mem.c
    portals4/gupcr_node.c
    portals4/gupcr_pgm_info.c
    portals4/gupcr_portals.c
    portals4/gupcr_runtime.c
    portals4/gupcr_shutdown.c
    portals4/gupcr_tick.c
    portals4/gupcr_utils.c
  )

  if(LIBUPC_NODE_LOCAL_MEM STREQUAL "mmap")
    list(APPEND LIBUPC_SOURCES portals4/gupcr_node_mem_mmap.c)
  else()
    list(APPEND LIBUPC_SOURCES portals4/gupcr_node_mem_posix.c)
  endif()


  list(APPEND LIBUPC_SOURCES
    #collectives/upc_coll_broadcast.upc
    collectives/upc_coll_err.upc
    collectives/upc_coll_exchange.upc
    collectives/upc_coll_gather.upc
    collectives/upc_coll_gather_all.upc
    #collectives/upc_coll_init.upc
    collectives/upc_coll_permute.upc
    collectives/upc_coll_prefix_reduce.upc
    #collectives/upc_coll_reduce.upc
    collectives/upc_coll_scatter.upc
    collectives/upc_coll_sort.upc
  )

else()
  message(fatal_error "Unknown value of LIBUPC_RUNTIME_MODEL")
endif()

foreach(config ${CONFIGURATIONS})
  set(pts_type PACKED)
  set(vaddr_order FIRST)
  set(packed_bits)

  string(REGEX MATCHALL "[^-]+" args ${config})
  
  # Somehow f is set to a variable, which
  # breaks because of the bizarre rules of
  # STREQUAL in CMake.
  set(f f)

  foreach(argument ${args})
    if(${argument} STREQUAL p)
      set(pts_type PACKED)
    elseif(${argument} STREQUAL s)
      set(pts_type STRUCT)
    elseif(${argument} STREQUAL f)
      set(vaddr_order FIRST)
    elseif(${argument} STREQUAL l)
      set(vaddr_order LAST)
    else()
      list(APPEND packed_bits ${argument})
    endif()
  endforeach()

  if(pts_type STREQUAL PACKED)
    if(packed_bits)
      list(GET packed_bits 0 phase)
      list(GET packed_bits 1 thread)
      list(GET packed_bits 2 addr)
    else()
      set(phase 20)
      set(thread 10)
      set(addr 34)
    endif()
  else()
    set(phase 32)
    set(thread 32)
    set(addr 64)
  endif()

  # calculate the library name
  set(lib_name "upc")
  if(${pts_type} STREQUAL STRUCT)
    set(lib_name ${lib_name}-s)
  endif()
  if(${vaddr_order} STREQUAL LAST)
    set(lib_name ${lib_name}-l)
  endif()
  if((${pts_type} STREQUAL PACKED) AND
     NOT (${phase} EQUAL 20 AND ${thread} EQUAL 10 AND ${addr} EQUAL 34))
    set(lib_name ${lib_name}-${phase}-${thread}-${addr})
  endif()

  # Compute the preprocessor definitions
  if(${pts_type} STREQUAL PACKED)
    set(lib_defs "GUPCR_PTS_PACKED_REP=1")
    set(flags "-fupc-pts=packed -fupc-packed-bits=${phase},${thread},${addr}")
  elseif(${pts_type} STREQUAL STRUCT)
    set(lib_defs "GUPCR_PTS_STRUCT_REP=1")
    set(flags "-fupc-pts=struct")
  endif()
  set(lib_defs ${lib_defs};GUPCR_PTS_PHASE_SIZE=${phase})
  set(lib_defs ${lib_defs};GUPCR_PTS_THREAD_SIZE=${thread})
  set(lib_defs ${lib_defs};GUPCR_PTS_VADDR_SIZE=${addr})
  if(${vaddr_order} STREQUAL FIRST)
    set(lib_defs ${lib_defs};GUPCR_PTS_VADDR_FIRST=1)
    set(flags "${flags} -fupc-pts-vaddr-order=first")
  else()
    set(flags "${flags} -fupc-pts-vaddr-order=last")
  endif()

  # Build in the target library
  set(lib_defs ${lib_defs};IN_TARGET_LIBS=1)

  # Build the library
  if(LIBUPC_ENABLE_SHARED)
    add_library(${lib_name} SHARED ${LIBUPC_SOURCES})
  else()
    add_library(${lib_name} STATIC ${LIBUPC_SOURCES})
  endif()

  set_property(TARGET ${lib_name} PROPERTY COMPILE_DEFINITIONS ${lib_defs})
  set_property(TARGET ${lib_name} PROPERTY COMPILE_FLAGS ${flags})

  add_dependencies(${lib_name} clang)
  add_dependencies(${lib_name} clang-upc-lib-h)
  add_dependencies(${lib_name} upc-headers)
  add_dependencies(${lib_name} upc-atomic)

  install(TARGETS ${lib_name}
    DESTINATION lib${LLVM_LIBDIR_SUFFIX})

endforeach()

set_property(SOURCE ${LIBUPC_SOURCES} PROPERTY LANGUAGE C)

if(APPLE)
  set(config_dir darwin)
else()
  set(config_dir default)
endif()

macro(add_crt_target name opts)
  set(obj ${LLVM_BINARY_DIR}/lib${LLVM_LIBDIR_SUFFIX}/${name}.o)
  list(APPEND objects ${obj})
  add_custom_target(${name} ALL DEPENDS ${obj})
  add_custom_command(OUTPUT ${obj}
  COMMAND ${CMAKE_C_COMPILER} -c -o ${obj} ${PROJECT_SOURCE_DIR}/upc-crtstuff.c -I${PROJECT_SOURCE_DIR}/config/${config_dir} -I${PROJECT_BINARY_DIR} ${opts}
  MAIN_DEPENDENCY upc-crtstuff.c
  IMPLICIT_DEPENDENCY upc-crtstuff.c
  VERBATIM)
  add_dependencies(${name} clang)
endmacro()

add_crt_target(upc-crtbegin -DCRT_BEGIN)
add_crt_target(upc-crtend -DCRT_END)
add_crt_target(upc-crtbeginS -DCRT_BEGIN)
add_crt_target(upc-crtendS -DCRT_END)
add_crt_target(upc-crtbeginT -DCRT_BEGIN)
add_crt_target(upc-crtendT -DCRT_END)

find_package(Perl REQUIRED)
find_program(AUTOGEN_EXECUTABLE autogen)

if(LIBUPC_ENABLE_LINK_SCRIPT)

set(upc_link_script ${LLVM_BINARY_DIR}/lib${LLVM_LIBDIR_SUFFIX}/upc.ld)
add_custom_target(upc-link-script ALL DEPENDS ${upc_link_script})
add_custom_command(OUTPUT ${upc_link_script}
   COMMAND ld --verbose | ${PERL_EXECUTABLE} ${PROJECT_SOURCE_DIR}/gen-upc-ld-script.pl > ${upc_link_script}
   VERBATIM)

endif()

set(header_location lib${LLVM_LIBDIR_SUFFIX}/clang/${CLANG_VERSION}/include)

set(clang_upc_lib_h ${LLVM_BINARY_DIR}/${header_location}/clang-upc-lib.h)
set(clang_upc_lib_h_sources ${PROJECT_SOURCE_DIR}/gen-inline-libupc.pl ${PROJECT_SOURCE_DIR}/smp/upc-lib.in
  ${PROJECT_BINARY_DIR}/config.h
  ${PROJECT_SOURCE_DIR}/smp/upc_access.c
  ${PROJECT_SOURCE_DIR}/smp/upc_access.h
  ${PROJECT_SOURCE_DIR}/smp/upc_config.h
  ${PROJECT_SOURCE_DIR}/smp/upc_defs.h
  ${PROJECT_SOURCE_DIR}/smp/upc_mem.h
  ${PROJECT_SOURCE_DIR}/smp/upc_pts.h
  ${PROJECT_SOURCE_DIR}/smp/upc_sup.h
  ${PROJECT_SOURCE_DIR}/smp/upc_sync.h)
add_custom_command(OUTPUT ${clang_upc_lib_h}
  COMMAND ${CMAKE_COMMAND} -E make_directory ${LLVM_BINARY_DIR}/${header_location} &&
    ${PERL_EXECUTABLE} ${clang_upc_lib_h_sources} > ${clang_upc_lib_h}
  DEPENDS ${clang_upc_lib_h_sources}
  VERBATIM)
add_custom_target(clang-upc-lib-h ALL DEPENDS ${clang_upc_lib_h})

set(lib_upc_atomic ${PROJECT_SOURCE_DIR}/smp/upc_atomic.upc)
set(lib_upc_atomic_sources ${PROJECT_SOURCE_DIR}/smp/upc_atomic.def
  ${PROJECT_SOURCE_DIR}/smp/upc_atomic.tpl
  ${PROJECT_SOURCE_DIR}/include/upc_types.def
  ${PROJECT_SOURCE_DIR}/include/upc_ops.def)
add_custom_command(OUTPUT ${lib_upc_atomic}
  COMMAND cd ${PROJECT_SOURCE_DIR}/smp &&
          ${AUTOGEN_EXECUTABLE} -L ../include upc_atomic.def
  DEPENDS ${lib_upc_atomic_sources}
  VERBATIM)
add_custom_target(upc-atomic ALL DEPENDS ${lib_upc_atomic})

set(upc_headers clang-upc.h upc.h upc_atomic.h upc_castable.h
  upc_collective.h upc_nb.h upc_strict.h upc_tick.h upc_types.h
  upc_relaxed.h)
set(upc_header_targets)
foreach( f ${upc_headers} )
  set( src ${PROJECT_SOURCE_DIR}/include/${f} )
  set( dst ${LLVM_BINARY_DIR}/${header_location}/${f} )
  add_custom_command(OUTPUT ${dst}
    DEPENDS ${src}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${src} ${dst}
    COMMENT "Copying clang's ${f}...")
  list(APPEND upc_header_targets ${dst})
endforeach( f )

add_custom_target(upc-headers ALL DEPENDS ${upc_header_targets})

install(FILES include/clang-upc.h include/upc.h include/upc_atomic.h
  include/upc_castable.h include/upc_collective.h include/upc_nb.h
  include/upc_strict.h include/upc_tick.h include/upc_types.h
  include/upc_relaxed.h
  DESTINATION ${header_location})

install(FILES ${objects} ${upc_link_script}
  DESTINATION lib${LLVM_LIBDIR_SUFFIX})

install(FILES ${clang_upc_lib_h}
  DESTINATION ${header_location})

#===============================================================================
# Setup Tests
#===============================================================================

# add_subdirectory(test)
