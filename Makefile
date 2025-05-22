SHELL := bash
.RECIPEPREFIX := >
CXX := g++
CFLAGS := `root-config --cflags`
WFLAGS := -Wall -Wpedantic
LDFLAGS := `root-config --libs`
SRCLIST := $(wildcard *.cxx)
OBJLIST := $(SRCLIST:.cxx=.o)

ifeq ($(PFLAG),prof)
CFLAGS+=-pg
else
CFLAGS+=-O3
endif

#RPATH := $(PWD)/../GRETINA-LBNL-gretina-unpack-aafb1b2/lib/
#INCDIR := $(PWD)/../GRETINA-LBNL-gretina-unpack-aafb1b2/include/
INCDIR := $(PWD)

CFLAGS+=-I$(INCDIR)

all: extract
#>    @rm *.o

extract: extract.o HistPlotter.o utilities_gt.cpp
>   @echo "Building executable.."
#>   $(CXX) $(CFLAGS) $^ $(LDFLAGS) $(WFLAGS) -L$(RPATH) -Wl,-rpath,$(RPATH) -o $@
>   $(CXX) $(CFLAGS) $^ $(LDFLAGS) $(WFLAGS) -o $@ -lz
>	rm *.o

%.o: %.cpp
>   $(CXX) $(CFLAGS) -c $^ $(LDFLAGS) $(WFLAGS)

clean:
>   rm -r extract *.o

.PHONY:
>   clean all cleanall extract
