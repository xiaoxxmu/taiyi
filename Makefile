CXX := g++

CXXFLAGS := -std=c++11 -Wall -Iinclude

LIBDIR := /D/GitHub/taiyi/lib
LDFLAGS := -L$(LIBDIR) -lthostmduserapi.dll -lthosttraderapi.dll

TARGET := taiyi.exe

SRCS := $(wildcard src/*.cpp)

OBJS := $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX)  $^ -o $@ $(LDFLAGS)

%.o: %.cpp 
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

