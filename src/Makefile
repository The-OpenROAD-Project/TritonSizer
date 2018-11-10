# Please set your environment
TCLLIB = -L ../module/tcl/unix/install-sp/lib -ltcl8.4
ZLIB_HOME = __YOUR_ZLIB_HOME__/lib

# Below references submodule 
OPENSTA_HOME = ../module/OpenSTA
OPENSTALIB_HOME = ../module/OpenSTA/install-sp


SUFFIX  = cpp
GCC = g++

OFLAGS = -Wall -fomit-frame-pointer -funroll-all-loops -m64 -fprefetch-loop-arrays -ffast-math 

ZLIB = -lz

OPENSTALIB = -Wl,--start-group $(OPENSTALIB_HOME)/lib/libliberty.a \
			$(OPENSTALIB_HOME)/lib/libutil.a \
			$(OPENSTALIB_HOME)/lib/libsearch.a \
			$(OPENSTALIB_HOME)/lib/libdcalc.a \
			$(OPENSTALIB_HOME)/lib/libgraph.a \
			$(OPENSTALIB_HOME)/lib/libnetwork.a \
			$(OPENSTALIB_HOME)/lib/libparasitics.a \
			$(OPENSTALIB_HOME)/lib/libsdc.a \
			$(OPENSTALIB_HOME)/lib/libsdf.a \
			$(OPENSTALIB_HOME)/lib/libverilog.a \
			-Wl,--end-group


IFLAG = -I.  -I$(ZLIB_HOME)/../include -I$(OPENSTALIB_HOME)/include
LINK = 
LFLAG = -fPIC $(LINK) $(TCLLIB) \
		$(OPENSTA_HOME)/app/StaApp_wrap.o \
		$(OPENSTA_HOME)/app/StaMain.o \
		$(OPENSTA_HOME)/app/TclInitVar.o \
		$(OPENSTALIB) -L$(ZLIB_HOME) $(ZLIB) \
		-lm -lpthread -ldl -Wl,-R/liab
BINPATH = .
OBJPATH = .

SOURCES = $(wildcard *.$(SUFFIX))
OBJECTS = $(SOURCES:%.$(SUFFIX)=$(OBJPATH)/%.o)
HEADERS = $(wildcard ./*.h)
CFLAGS = $(OFLAG) $(IFLAG) -c -std=c++14 -fPIC -Wno-deprecated -g -gstrict-dwarf $(DBG) -Wunused -fpermissive

TARGET = $(BINPATH)/sizer$(EXPER)

$(TARGET):$(OBJECTS)
	$(PY) $(GCC) -o $(TARGET) $(IFLAG) \
	$(OBJECTS) \
	$(LFLAG) \
	-g -gstrict-dwarf 

# Compile the application code
$(OBJPATH)/%.o: %.$(SUFFIX) $(HEADERS)
	$(PY) $(GCC) -o $@ $(CFLAGS) $< 
.KEEP_STATE:
test:
	@echo "Test ... "
	@echo "Match! :-)"
	@echo "Not match! :-("
clean:
	@echo "Now Removing ..."
	@-rm -rf $(OBJECTS)
	@echo "Done!"
profile:
	@$(MAKE) -f makefile DBG=""
debug:
	@$(MAKE) -f makefile DBG="-g"
opt:
	@$(MAKE) -f makefile DBG="-O3"
release:
	@$(MAKE) -f makefile LINK="-static-libstdc++"
