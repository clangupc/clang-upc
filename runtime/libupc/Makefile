##===- runtime/libupc/Makefile -----------------*- Makefile -*-===##

CLANG_LEVEL=../..

LIBRARYNAME=upc

include Makefile.optional

ifeq ($(LIBUPC_RUNTIME_MODEL),smp)
SOURCES=\
	upc_access.c\
	upc_access.c\
	upc_accessg.c\
	upc_addr.c\
	upc_atomic_sup.c\
	upc_debug.c\
	upc_gasp.c\
	upc_gum.c\
	upc_libg.c\
	upc_main.c\
	upc_mem.c\
	upc_nb.upc\
	upc_pgm_info.c\
	upc_pupc.c\
	upc_sysdep.c\
	upc_tick.c\
	upc_vm.c

SOURCES +=\
	upc_allocg.upc\
	upc_alloc.upc\
	upc_atomic_generic.upc\
	upc_barrier.upc\
	upc_castable.upc\
	upc_lock.upc

ifeq ($(LIBUPC_ENABLE_BACKTRACE),1)
SOURCES +=\
	upc_backtrace.c
endif
ifeq ($(LIBUPC_ENABLE_AFFINITY),1)
  SOURCES_AFFINITY = upc_affinity.c
else
  SOURCES_AFFINITY = upc_affinity_stub.c
endif
ifeq ($(LIBUPC_ENABLE_NUMA),1)
  SOURCES_NUMA = upc_numa.c
else
  SOURCES_NUMA = upc_numa_stub.c
endif
SOURCES += $(SOURCES_NUMA) $(SOURCES_AFFINITY)

SOURCES +=\
	upc_coll_broadcast.upc \
	upc_coll_exchange.upc \
	upc_coll_gather.upc \
	upc_coll_gather_all.upc \
	upc_coll_init.upc \
	upc_coll_permute.upc \
	upc_coll_prefix_reduce.upc \
	upc_coll_reduce.upc \
	upc_coll_scatter.upc \
	upc_coll_sort.upc

SOURCES_INLINE = config.h upc_access.c upc_access.h \
	upc_config.h upc_defs.h upc_mem.h upc_pts.h \
	upc_sup.h upc_sync.h upc_sysdep.h

else ifeq ($(LIBUPC_RUNTIME_MODEL),portals4)

SOURCES =\
	gupcr_access.c \
	gupcr_addr.c \
	gupcr_atomic_sup.c \
	gupcr_atomic.upc \
	gupcr_barrier.c \
	gupcr_backtrace.c \
	gupcr_broadcast.c \
	gupcr_clock.c \
	gupcr_coll_sup.c \
	gupcr_env.c \
	gupcr_gmem.c \
	gupcr_lock_sup.c \
	gupcr_main.c \
	gupcr_mem.c \
	gupcr_nb_sup.c \
	gupcr_node.c \
	gupcr_pgm_info.c \
	gupcr_portals.c \
	gupcr_runtime.c \
	gupcr_shutdown.c \
	gupcr_tick.c \
	gupcr_utils.c

SOURCES +=\
	gupcr_alloc.upc \
	gupcr_lock.upc \
	gupcr_nb.upc \
	gupcr_coll_broadcast.upc \
	gupcr_castable.upc \
	gupcr_coll_init.upc \
	gupcr_coll_reduce.upc

ifeq ($(LIBUPC_NODE_LOCAL_MEM STREQUAL),mmap)
SOURCES += gupcr_node_mem_mmap.c
else
SOURCES += gupcr_node_mem_posix.c
endif

SOURCES +=\
	upc_coll_err.upc \
	upc_coll_exchange.upc \
	upc_coll_gather.upc \
	upc_coll_gather_all.upc \
	upc_coll_permute.upc \
	upc_coll_prefix_reduce.upc \
	upc_coll_scatter.upc \
	upc_coll_sort.upc

SOURCES_INLINE = config.h gupcr_access.c gupcr_access.h gupcr_config.h \
	gupcr_defs.h gupcr_gmem.h gupcr_node.h gupcr_portals.h \
	gupcr_pts.h gupcr_sup.h gupcr_sync.h gupcr_utils.h

else ifeq ($(LIBUPC_RUNTIME_MODEL),fabric)

SOURCES =\
	gupcr_access.c \
	gupcr_addr.c \
	gupcr_atomic_sup.c \
	gupcr_atomic.upc \
	gupcr_barrier.c \
	gupcr_backtrace.c \
	gupcr_broadcast.c \
	gupcr_clock.c \
	gupcr_coll_sup.c \
	gupcr_env.c \
	gupcr_gmem.c \
	gupcr_lock_sup.c \
	gupcr_main.c \
	gupcr_mem.c \
	gupcr_nb_sup.c \
	gupcr_node.c \
	gupcr_pgm_info.c \
	gupcr_fabric.c \
	gupcr_runtime.c \
	gupcr_shutdown.c \
	gupcr_tick.c \
	gupcr_utils.c

SOURCES +=\
	gupcr_alloc.upc \
	gupcr_lock.upc \
	gupcr_nb.upc \
	gupcr_coll_broadcast.upc \
	gupcr_castable.upc \
	gupcr_coll_init.upc \
	gupcr_coll_reduce.upc

ifeq ($(LIBUPC_NODE_LOCAL_MEM STREQUAL),mmap)
SOURCES += gupcr_node_mem_mmap.c
else
SOURCES += gupcr_node_mem_posix.c
endif

SOURCES +=\
	upc_coll_err.upc \
	upc_coll_exchange.upc \
	upc_coll_gather.upc \
	upc_coll_gather_all.upc \
	upc_coll_permute.upc \
	upc_coll_prefix_reduce.upc \
	upc_coll_scatter.upc \
	upc_coll_sort.upc

SOURCES_INLINE = config.h gupcr_access.c gupcr_access.h gupcr_config.h \
	gupcr_defs.h gupcr_gmem.h gupcr_node.h gupcr_fabric.h \
	gupcr_pts.h gupcr_sup.h gupcr_sync.h gupcr_utils.h

else
$(error Wrong runtime model ($(LIBUPC_RUNTIME_MODEL)) or it is not specified)
endif
 
include $(CLANG_LEVEL)/Makefile
include Makefile.rules
NO_MAYBE_UNINITIALIZED=

LibDir := $(PROJ_OBJ_ROOT)/$(BuildMode)/lib
PROJ_lib := $(DESTDIR)$(PROJ_prefix)/lib

# Build and install link script
ifeq ($(LIBUPC_LINK_SCRIPT),1)
UPC_LINK_SCRIPT := upc.ld                                                       
$(UPC_LINK_SCRIPT): gen-upc-ld-script.pl                                        
	$(Verb) echo "int main(){}" | $(CC) $(LDFLAGS) -Xlinker --verbose -o /dev/null -xc - | \
	$(PERL) $+ > $@

BUILT_SOURCES += $(UPC_LINK_SCRIPT)

UPC_LD_BUILD := $(addprefix $(LibDir)/, upc.ld)
$(UPC_LD_BUILD): upc.ld
	$(Verb) cp $< $@
	$(Echo) Copying $(notdir $<) to build dir

all-local:: $(UPC_LD_BUILD)

UPC_LD_INSTALL := $(PROJ_lib)/upc.ld
 
$(UPC_LD_INSTALL): $(UPC_LD_BUILD)
	$(Verb) $(DataInstall) $< $(UPC_LD_INSTALL)
	$(Echo) Installing compiler linker file: $(notdir $<)

install-local:: $(UPC_LD_INSTALL)
endif

# Build and install various crt files
OS := $(shell uname -s)
ifeq ($(OS),Darwin)
  CFG_DIR := darwin
else
  CFG_DIR := default
endif
CRT_INC := $(PROJ_SRC_DIR)/config/$(CFG_DIR)
upc-crtbegin.o: $(PROJ_SRC_DIR)/upc-crtstuff.c
	$(Verb) $(Compile.C) -Wno-empty-translation-unit -DCRT_BEGIN -I$(CRT_INC) $< -o $@
upc-crtbeginS.o: $(PROJ_SRC_DIR)/upc-crtstuff.c
	$(Verb) $(Compile.C) -Wno-empty-translation-unit -DCRT_BEGIN -I$(CRT_INC) $< -o $@
upc-crtbeginT.o: $(PROJ_SRC_DIR)/upc-crtstuff.c
	$(Verb) $(Compile.C) -Wno-empty-translation-unit -DCRT_BEGIN -I$(CRT_INC) $< -o $@
upc-crtend.o: $(PROJ_SRC_DIR)/upc-crtstuff.c
	$(Verb) $(Compile.C) -Wno-empty-translation-unit -DCRT_END -I$(CRT_INC) $< -o $@
upc-crtendS.o: $(PROJ_SRC_DIR)/upc-crtstuff.c
	$(Verb) $(Compile.C) -Wno-empty-translation-unit -DCRT_END -I$(CRT_INC) $< -o $@
upc-crtendT.o: $(PROJ_SRC_DIR)/upc-crtstuff.c
	$(Verb) $(Compile.C) -Wno-empty-translation-unit -DCRT_END -I$(CRT_INC) $< -o $@

CRT_FILES := upc-crtbegin.o upc-crtbeginS.o upc-crtbeginT.o \
	upc-crtend.o upc-crtendS.o upc-crtendT.o

CRT_BLD_FILES := $(addprefix $(LibDir)/, $(CRT_FILES))

$(CRT_BLD_FILES): $(LibDir)/%.o: %.o $(LibDir)/.dir
	$(Verb) cp $< $@
	$(Echo) Copying $(notdir $<) to build dir

CleanCrtFiles:
	$(Verb) rm -f $(CRT_FILES)

CleanBldCrtFiles:
	$(Verb) rm -f $(CRT_BLD_FILES)

all-local:: $(CRT_BLD_FILES)
clean-local:: CleanCrtFiles
clean:: CleanBldCrtFiles

INSTCRTS := $(addprefix $(PROJ_lib)/, $(CRT_FILES))

$(INSTCRTS): $(PROJ_lib)/%.o: %.o
	$(Verb) $(DataInstall) $< $(PROJ_lib)
	$(Echo) Installing compiler include file: $(notdir $<)

install-local:: $(INSTCRTS)

# Install headers (based on the code in lib/Headers)
CLANG_VERSION := $(word 3,$(shell grep "CLANG_VERSION " \
	$(PROJ_OBJ_DIR)/$(CLANG_LEVEL)/include/clang/Basic/Version.inc))

HeaderDir := $(PROJ_OBJ_ROOT)/$(BuildMode)/lib/clang/$(CLANG_VERSION)/include

HEADERS := $(notdir $(wildcard $(PROJ_SRC_DIR)/include/*.h))

OBJHEADERS := $(addprefix $(HeaderDir)/, $(HEADERS))

$(OBJHEADERS): $(HeaderDir)/%.h: $(PROJ_SRC_DIR)/include/%.h $(HeaderDir)/.dir
	$(Verb) cp $< $@
	$(Echo) Copying $(notdir $<) to build dir

CLANG_UPC_LIB_HEADER := $(addprefix $(HeaderDir)/, clang-upc-lib.h)
$(CLANG_UPC_LIB_HEADER): gen-inline-libupc.pl upc-lib.in $(SOURCES_INLINE)
	$(Verb) $(PERL) $+ > $@
	$(Echo) Building clang-upc-lib.h

BUILT_SOURCES = $(CLANG_UPC_LIB_HEADER)

all-local:: $(OBJHEADERS) $(CLANG_UPC_LIB_HEADER)

PROJ_headers := $(DESTDIR)$(PROJ_prefix)/lib/clang/$(CLANG_VERSION)/include

INSTHEADERS := $(addprefix $(PROJ_headers)/, $(HEADERS))
INSTHEADERS += $(PROJ_headers)/clang-upc-lib.h

$(PROJ_headers):
	$(Verb) $(MKDIR) $@

$(INSTHEADERS): $(PROJ_headers)/%.h: $(HeaderDir)/%.h | $(PROJ_headers)
	$(Verb) $(DataInstall) $< $(PROJ_headers)
	$(Echo) Installing compiler include file: $(notdir $<)

install-local:: $(INSTHEADERS)

COMPILE_DEPS += $(OBJHEADERS)

# UPC compiling
Compile.UPC = $(Compile.Wrapper) \
	$(Verb) $(UPC) $(CPP.Flags) $(C.Flags) $(CFLAGS) $(CPPFLAGS) \
	$(TargetCommonOpts) $(CompileCommonOpts) -c

$(ObjDir)/%.o: %.upc $(ObjDir)/.dir $(BUILT_SOURCES) $(OBJHEADERS)
	$(Echo) "Compiling $*.upc for $(BuildMode) build" $(PIC_FLAG)
	$(Compile.UPC) $< -o $@


