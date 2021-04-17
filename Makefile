lsmSrc = $(wildcard src/lsm/*.cpp)
lsmIncludes = $(wildcard src/lsm/*.hpp) src/diskSectors.h

computeSrc = $(wildcard src/compute/*.c)
computeIncludes = $(wildcard src/compute/*.h) src/diskSectors.h

all: lsm compute disk

lsm: $(lsmSrc) $(lsmIncludes)
	g++ -fpermissive -std=c++11 -O3 $(lsmSrc) -o lsm -lpthread

compute: $(computeSrc) $(computeIncludes)
	gcc $(computeSrc) -o compute

disk:
	mkdir -p disk

clean:
	rm -f lsm compute