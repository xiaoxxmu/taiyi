CXX := g++

CXXFLAGS := -std=c++11 -Wall

INCLUDE_COMMON = src/common
INCLUDE_CTPAPI = deps/ctp_api
INCLUDE = -I$(INCLUDE_COMMON) -I$(INCLUDE_CTPAPI)

LIBDIR := /usr/local/bin
CTP_LIB_DIR := deps/ctp_api/linux_x64
LDFLAGS := -L$(LIBDIR) -L$(CTP_LIB_DIR) -pthread -lthostmduserapi_se -lthosttraderapi_se

TARGET := test

SRCS := $(wildcard src/*.cc src/common/*.cc src/store/*.cc src/md/*.cc src/trade/*.cc src/ctp_service/*.cc)

OBJS := $(SRCS:.cc=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) $^ -o $@

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(INCLUDE)

clean:
	rm -f $(OBJS) $(TARGET)
