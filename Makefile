CC=g++
CFLAGS=-O3 -Wall
LDFLAGS=-L /usr/lib64/atlas-sse3/ -lptcblas -lopencv_core -lopencv_highgui
SOURCES=eHbbox.cpp eHfeatpyramid.cpp eHimageFeature.cpp eHutils.cpp eHbox.cpp eHfilter.cpp eHmatrix.cpp eHfacemodel.cpp eHimage.cpp eHshiftdt.cpp main.cpp
OBJECTS=
EXECUTABLE=test.64

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(SOURCES)
	$(CC) $(CFLAGS) $(LDFLAGS) $(SOURCES) -o $(EXECUTABLE)

clean: 
	rm -rf $(EXECUTABLE)
