CC := gcc
AR := ar
CFLAGS := -Wall -O3
MKDIR := mkdir -p
CP := cp -a
RM := rm -rf

ifeq ($(OS),Windows_NT)
    HOST_OS := WIN32
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Darwin)
        HOST_OS := MACOS
    else
        HOST_OS := LINUX
    endif
endif

ifeq ($(HOST_OS),WIN32)
    RC := windres
    RCFLAGS := --define GCC_WINDRES
endif

VER_MAJOR := 1
VER_MINOR := 1
VER_PATCH := 2

#ifneq ($(HOST_OS),WIN32)
#    VER_MAJOR := $(shell echo `sed -n -e '/VERSION "/s/.*"\([[:digit:]]\{1,\}\)\.[[:digit:]]\{1,\}\.[[:digit:]]\{1,\}".*/\1/p' date_converter.h`)
#    VER_MINOR := $(shell echo `sed -n -e '/VERSION "/s/.*"[[:digit:]]\{1,\}\.\([[:digit:]]\{1,\}\)\.[[:digit:]]\{1,\}".*/\1/p' date_converter.h`)
#    VER_PATCH := $(shell echo `sed -n -e '/VERSION "/s/.*"[[:digit:]]\{1,\}\.[[:digit:]]\{1,\}\.\([[:digit:]]\{1,\}\)".*/\1/p' date_converter.h`)
#endif

#-------------------------------------------------------------------------
# 'make install' and 'make clean' are validated only for Linux and macOS
#-------------------------------------------------------------------------

# Where do you want it installed when you do 'make install'
PREFIX ?= /usr/local
prefix ?=
ifneq ($(strip $(prefix)),)
    PREFIX = $(prefix)
endif

LWE_NAME = libdateconv
STATIC_LIB_NAME = $(LWE_NAME).a

SHARED_OPTION = -shared

ifeq ($(HOST_OS),WIN32)
    SHLIB_EXT = .dll
    SHLIB_EXT_IMPORT = .dll.a
    SHARED_LIB_NAME = $(LWE_NAME)-$(VER_MAJOR)$(SHLIB_EXT)
    COMPILER_OPTIONS = -Wl,--out-implib,$(LWE_NAME)$(SHLIB_EXT_IMPORT)
else ifeq ($(HOST_OS),MACOS)
    SHARED_OPTION = -dynamiclib
    SHLIB_EXT = .dylib
    SHARED_LIB_NAME = $(LWE_NAME).$(VER_MAJOR).$(VER_MINOR).$(VER_PATCH)$(SHLIB_EXT)
    LIB_NAME_SYM_S = $(LWE_NAME).$(VER_MAJOR)$(SHLIB_EXT)
    LIB_NAME_SYM_L = $(LWE_NAME)$(SHLIB_EXT)
    COMPILER_OPTIONS = -Wl,-install_name,$(LIB_NAME_SYM_S) -Wl,-compatibility_version,$(VER_MAJOR).0.0 -Wl,-current_version,$(VER_MAJOR).$(VER_MINOR).$(VER_PATCH)
else ifeq ($(HOST_OS),LINUX)
    SHLIB_EXT = .so
    SHARED_LIB_NAME = $(LWE_NAME)$(SHLIB_EXT).$(VER_MAJOR).$(VER_MINOR).$(VER_PATCH)
    LIB_NAME_SYM_S = $(LWE_NAME)$(SHLIB_EXT).$(VER_MAJOR)
    LIB_NAME_SYM_L = $(LWE_NAME)$(SHLIB_EXT)
    COMPILER_OPTIONS = -Wl,-soname,$(LIB_NAME_SYM_S)
endif

all: shared static

date_converter.o: date_converter.c
	$(CC) $(CFLAGS) -c -o $@ $<

shared: date_converter.c
ifeq ($(HOST_OS),WIN32)
	$(CC) $(CFLAGS) -c -o date_converter.o $<
	$(RC) $(RCFLAGS) -o libdateconv_wrc.o libdateconv_w.rc
	$(CC) $(CFLAGS) $(SHARED_OPTION) $(COMPILER_OPTIONS) -o $(SHARED_LIB_NAME) date_converter.o libdateconv_wrc.o
else
	-@$(MKDIR) .libs
	$(CC) $(CFLAGS) -fPIC -c -o .libs/date_converter.o $<
	$(CC) $(CFLAGS) -fPIC $(SHARED_OPTION) $(COMPILER_OPTIONS) -o $(SHARED_LIB_NAME) .libs/date_converter.o -lc -lm
	-$(RM) $(LIB_NAME_SYM_S) $(LIB_NAME_SYM_L)
	ln -s $(SHARED_LIB_NAME) $(LIB_NAME_SYM_S)
	ln -s $(SHARED_LIB_NAME) $(LIB_NAME_SYM_L)
	chmod 755 $(SHARED_LIB_NAME)
endif

static: date_converter.o
	$(AR) rcs $(STATIC_LIB_NAME) $<
ifneq ($(HOST_OS),WIN32)
	chmod 644 $(STATIC_LIB_NAME)
endif

install:
	$(MKDIR) $(PREFIX)/include
	$(MKDIR) $(PREFIX)/lib/pkgconfig
ifeq ($(HOST_OS),WIN32)
	$(MKDIR) $(PREFIX)/bin
endif

	$(CP) date_converter.h $(PREFIX)/include
	$(CP) *.a $(PREFIX)/lib 2>/dev/null || :
ifeq ($(HOST_OS),WIN32)
	$(CP) *$(SHLIB_EXT) $(PREFIX)/bin 2>/dev/null || :
else
	$(CP) *$(SHLIB_EXT)* $(PREFIX)/lib 2>/dev/null || :
	chmod 644 $(PREFIX)/include/date_converter.h
endif

	sed \
		-e 's|@prefix@|${PREFIX}|g' \
		-e 's|@VERSION@|'`sed -n -e '/VERSION "/s/.*"\(.*\)".*/\1/p' date_converter.h`'|g' \
		libdateconv.pc.in > $(PREFIX)/lib/pkgconfig/libdateconv.pc

clean:
	-$(RM) *.o *.a *$(SHLIB_EXT) test .libs
ifeq ($(HOST_OS),LINUX)
	-$(RM) $(SHARED_LIB_NAME) $(LIB_NAME_SYM_S)
endif

test: tests/test.c
ifeq ($(HOST_OS),WIN32)
	$(CC) $(CFLAGS) $< -I. -L. -ldateconv -o $@
else
	$(CC) $(CFLAGS) -Wl,-rpath,'$$ORIGIN' $< -I. -L. -ldateconv -lm -o $@
endif
