
ALLFLAGS = -Wall -Wextra -pedantic -g -pthread

CXXFLAGS = $(ALLFLAGS)
CPPFLAGS = $(CXXFLAGS) -std=c++17

LINKFlAGS = $(ALLFLAGS)

MPICPP = mpic++

ALLCPP = $(wildcard *.cpp)
ALLCPPOBJS = $(ALLCPP:%.cpp=%.o)

%.o: %.cpp
	$(MPICPP) -c -o $@ $< $(CPPFLAGS)

PsoSpp: $(ALLCPPOBJS)
	$(MPICPP) -o $@ $^ $(LINKFlAGS)