CXX := g++

CXXFLAGS := -std=c++11 -Wall

INCLUDE_COMMON = src/common
INCLUDE = -I$(INCLUDE_COMMON)

LIBDIR := /usr/local/bin
LDFLAGS := -L$(LIBDIR) -pthread

TARGET := test

SRCS := $(wildcard src/*.cc src/common/*.cc src/store/*.cc src/md/*.cc src/trade/*.cc)

OBJS := $(SRCS:.cc=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) $^ -o $@

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(INCLUDE)

clean:
	rm -f $(OBJS) $(TARGET)
