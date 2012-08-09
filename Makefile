CC=g++
CFLAGS=-O3 -Wall
LDFLAGS_BLAS=-L /usr/lib64/atlas-sse3/ -lptcblas
#LDFLAGS_BLAS=-lblas
LDFLAGS_OPENCV=-lopencv_core -lopencv_highgui
SOURCES=eHbbox.cpp eHfeatpyramid.cpp eHimageFeature.cpp eHutils.cpp eHbox.cpp eHfilter.cpp eHmatrix.cpp eHfacemodel.cpp eHimage.cpp eHshiftdt.cpp main.cpp
OBJECTS=
EXECUTABLE=test.64

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(SOURCES)
	$(CC) -o $(EXECUTABLE) $(SOURCES) $(CFLAGS) $(LDFLAGS_BLAS) $(LDFLAGS_OPENCV)

clean: 
	rm -rf $(EXECUTABLE)
