CFLAGS := -O3 -Wall $(CFLAGS)

OPENCV_CFLAGS = `pkg-config --cflags opencv`
OPENCV_LIBS = `pkg-config --libs opencv`

SOURCES = Superpixels.cpp Superpixels.hpp main.cpp

all : $(SOURCES)
	g++ -g -std=c++11 $(OPENCV_CFLAGS) $(CFLAGS) -lm -o superpixels $(SOURCES) $(OPENCV_LIBS) $(LDFLAGS)	

test: SLIC.cpp
	g++ -g -std=c++11 $(OPENCV_CFLAGS) $(CFLAGS) -lm -o slic SLIC.cpp $(OPENCV_LIBS) $(LDFLAGS)	
