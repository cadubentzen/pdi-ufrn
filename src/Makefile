CXX = g++
CXXFLAGS = `pkg-config --cflags --libs opencv` -std=c++11

SOURCES = regions.cpp \
		  swap_regions.cpp \
		  bubbles.cpp \
		  equalize.cpp \
		  motiondetector.cpp \
		  laplgauss.cpp \
		  tiltshift.cpp \
		  tiltshiftvideo.cpp

all: $(addprefix bin/,$(basename $(SOURCES)))

bin/%: %.cpp bin
	$(CXX) $< -o $@ $(CXXFLAGS)

clean:
	-rm -rf bin

bin:
	-@mkdir bin
