CC=g++
CFLAGS= -g -std=c++11 -O3 -Wall -pedantic `pkg-config --cflags opencv`
LDFLAGS_BLAS=-lblas 
LDFLAGS=-lpthread `pkg-config --libs opencv` -lpqxx -lpq -lyaml-cpp
SOURCES=main.cpp FaceDetection.cpp eHbbox.cpp eHfeatpyramid.cpp eHimageFeature.cpp eHutils.cpp eHbox.cpp eHfilter.cpp eHmatrix.cpp eHfacemodel.cpp eHimage.cpp eHshiftdt.cpp eHposemodel.cpp
SOURCES_SINGLE=main_single.cpp FaceDetection.cpp eHbbox.cpp eHfeatpyramid.cpp eHimageFeature.cpp eHutils.cpp eHbox.cpp eHfilter.cpp eHmatrix.cpp eHfacemodel.cpp eHimage.cpp eHshiftdt.cpp eHposemodel.cpp
HEADERS=eHbbox.h FaceDetection.hpp eHfeatpyramid.h eHimageFeature.h eHutils.h eHbox.h eHfilter.h eHmatrix.h eHfacemodel.h eHimage.h eHshiftdt.h eHposemodel.cpp
OBJ = ${SOURCES:.cpp=.o}
OBJ_SINGLE = ${SOURCES_SINGLE:.cpp=.o}
EXECUTABLE=facefinder
EXECUTABLE_SINGLE=single-detect

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE) : ${OBJ}
	@echo CC -o $@
	$(CC) -o $(EXECUTABLE) ${OBJ} $(LDFLAGS) $(LDFLAGS_BLAS) 

$(EXECUTABLE_SINGLE) : ${OBJ_SINGLE}
	@echo CC -o $@
	$(CC) -o $(EXECUTABLE_SINGLE) ${OBJ_SINGLE} $(LDFLAGS) $(LDFLAGS_BLAS) 

.cpp.o:
	@echo CC $<
	@${CC} -c ${CFLAGS} $<

clean:
	rm -rf *.o $(EXECUTABLE)



# FEDORA
#LDFLAGS_BLAS=-L /usr/lib64/atlas-sse3/ -lptcblas
#LDFLAGS=-lpthread -lopencv_core -lopencv_highgui
#OBJECTS=$(SOURCES:.cpp=.o)
#$(EXECUTABLE): $(OBJECTS)
#	$(CC) $(LDFLAGS) $(LDFLAGS_BLAS) $(OBJECTS) -o $@

#.cpp.o:
#	$(CC) $(CFLAGS) $< -o $@

#clean: 
#	rm -rf *.o $(EXECUTABLE)
