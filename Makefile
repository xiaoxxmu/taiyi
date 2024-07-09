CXX := g++

CXXFLAGS := -std=c++11 -Wall

INCLUDE_CTPAPI = deps/ctp_api
INCLUDE = -I$(INCLUDE_CTPAPI)

LIBDIR := /usr/local/bin
#CTP_LIB_DIR := deps/ctp_api/linux_x64
#LDFLAGS := -L$(LIBDIR) -L$(CTP_LIB_DIR) -pthread -lthostmduserapi_se -lthosttraderapi_se
CTP_LIB_DIR := deps/ctp_api/linux_x64_openctp
LDFLAGS := -L$(LIBDIR) -L$(CTP_LIB_DIR) -pthread -lsoptthostmduserapi_se -lsoptthosttraderapi_se

#TARGET := client_taiyi_1.0.0
TARGET := my_trader

SRCS := $(wildcard src/*.cc)

OBJS := $(SRCS:.cc=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) $^ -o $@

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(INCLUDE)

clean:
	rm -f $(OBJS) $(TARGET)
