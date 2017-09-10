CXX = g++
CXXFLAGS = `pkg-config --cflags --libs opencv` -g

SOURCES = regions.cpp \
		  swap_regions.cpp \
		  bubbles.cpp \
		  equalize.cpp \
		  motiondetector.cpp \
		  laplgauss.cpp

OUTPUTS = $(basename $(SOURCES))

all: $(OUTPUTS)

%: %.cpp
	$(CXX) $< -o bin/$@ $(CXXFLAGS)

clean:
	-rm -rf bin/*
