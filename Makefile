CC=g++
CFLAGS=-O3 -Wall
#CFLAGS =-O0 -Wall -g
LDFLAGS_BLAS=-L /usr/lib64/atlas-sse3/ -lptcblas
#LDFLAGS_BLAS=-lblas
LDFLAGS_OPENCV=-lopencv_core -lopencv_highgui
SOURCES=eHbbox.cpp eHfeatpyramid.cpp eHimageFeature.cpp eHutils.cpp eHbox.cpp eHfilter.cpp eHmatrix.cpp eHfacemodel.cpp eHimage.cpp eHshiftdt.cpp eHposemodel.cpp main.cpp
HEADERS=eHbbox.h eHfeatpyramid.h eHimageFeature.h eHutils.h eHbox.h eHfilter.h eHmatrix.h eHfacemodel.h eHimage.h eHshiftdt.h eHposemodel.cpp
OBJECTS=
EXECUTABLE=test.64

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(SOURCES) $(HEADERS)
	$(CC) -o $(EXECUTABLE) $(SOURCES) $(CFLAGS) $(LDFLAGS_BLAS) $(LDFLAGS_OPENCV)

clean: 
	rm -rf $(EXECUTABLE)
