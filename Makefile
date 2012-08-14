CC=g++
CFLAGS=-O3 -Wall
#CFLAGS =-O0 -Wall -g
LDFLAGS_BLAS=-L /usr/lib64/atlas-sse3/ -lptcblas
#LDFLAGS_BLAS=-lblas
LDFLAGS=-lpthread -lopencv_core -lopencv_highgui
SOURCES=main.cpp eHbbox.cpp eHfeatpyramid.cpp eHimageFeature.cpp eHutils.cpp eHbox.cpp eHfilter.cpp eHmatrix.cpp eHfacemodel.cpp eHimage.cpp eHshiftdt.cpp eHposemodel.cpp
HEADERS=eHbbox.h eHfeatpyramid.h eHimageFeature.h eHutils.h eHbox.h eHfilter.h eHmatrix.h eHfacemodel.h eHimage.h eHshiftdt.h eHposemodel.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=test.64

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE) : $(SOURCES)
	$(CC) -o $(EXECUTABLE) $(SOURCES) $(CFLAGS) $(LDFLAGS) $(LDFLAGS_BLAS) 

clean:
	rm -rf *.o $(EXECUTABLE)

#$(EXECUTABLE): $(OBJECTS)
#	$(CC) $(LDFLAGS) $(LDFLAGS_BLAS) $(OBJECTS) -o $@

#.cpp.o:
#	$(CC) $(CFLAGS) $< -o $@

#clean: 
#	rm -rf *.o $(EXECUTABLE)
