include $(PROJECT_HOME)/mk_rules.mak

# ====================================================
# Compiler parameter
# ====================================================

# libraries to pass to the linker, e.g. -l<library>
LIBS += -lobj -lcommon -lssl -lcrypto -lpthread -lcurl -lstdc++ -lcjson

# linker directory, e.g. -L<lib dir>
LIB_DIR += -L$(USR_LIB_PATH)

# C/C++/Objective C preprocessor flags, e.g. -I<include dir> if you have headers in a nonstandard directory <include dir>
#INCLUDE += -I../../include/
INCLUDE = -I../../include/ -I$(LINUX_INCLUDE_DIR) -I$(APP_INCLUDE_DIR) 

# C compiler flags
CFLAGS += -Os -Wall

# debug flag
DEBUG += 

# ar flag,
# # r -> replace existing or insert new file(s) into the archive
# # c -> do not warn if the library had to be created
AR_FLAGS +=


# ====================================================
# define
# ====================================================

DEPEND_OBJS = curl_SOCOMEC.o

DEPEND_C =

DEPENDS = curl_SOCOMEC

# ====================================================
# common targets
# ====================================================

all: $(DEPENDS) 

install: all check_mkrules
	for i in $(DEPENDS) ; do \
		cp -f $$i $(BIN_PATH) || exit 1; \
	done

all_clean: clean

clean:
	rm -f --preserve-root *.o $(DEPENDS) || exit 1

distclean: check_mkrules clean
	for i in $(DEPENDS) ; do \
		rm -f --preserve-root $(BIN_PATH)/$$i || exit 1; \
	done
	
check_mkrules:
	@if [ -z ${PROJECT_HOME} ]; then \
		echo Have to source runfirst.sh ! && exit 1; \
	fi
	@if [ -z ${MKRULES_INCLUDED} ]; then \
		echo Have to include mk_rules.mak ! && exit 1; \
	fi

# ====================================================
# execution targets
# ====================================================

curl_SOCOMEC: $(DEPEND_OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(EXTRA_CFLAGS) $(ARCH) $(LDFLAGS) $(LIB_DIR) $(LIBS)
	$(STRIP) $@

# ====================================================
# .c.o 
# ====================================================
.c.o:
	$(CC) -c $(INCLUDE) $(DEBUG) $(CFLAGS) $<
