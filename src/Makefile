CXX = g++
CXXFLAGS = `pkg-config --cflags --libs opencv`

SOURCES = regions.cpp \
		  bubbles.cpp
OUTPUTS = $(basename $(SOURCES))

all: $(OUTPUTS)

%: %.cpp
	$(CXX) $< -o bin/$@ $(CXXFLAGS)

clean:
	-rm -rf bin/*
