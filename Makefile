CXX := g++

CXXFLAGS := -std=c++11 -Wall -Iapi

LIBDIR := api/linux_x64
LDFLAGS := -L$(LIBDIR) -lthostmduserapi_se -lthosttraderapi_se

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

