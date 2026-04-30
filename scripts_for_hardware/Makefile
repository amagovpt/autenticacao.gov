CXX ?= g++

TARGET := cc_reader
SOURCES := main.cpp public_info.cpp address_info.cpp
OBJECTS := $(SOURCES:.cpp=.o)

SDK_ROOT ?= ../pteid-mw-pt/_src/eidmw
SDK_INCLUDE ?= $(SDK_ROOT)/eidlib
SDK_LIB ?= $(SDK_ROOT)/lib
PTEID_LIB ?= pteidlib

CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -pedantic
CPPFLAGS += -I. -I$(SDK_INCLUDE) -I$(SDK_ROOT)/common -I$(SDK_ROOT)/cardlayer -I$(SDK_ROOT)/applayer -I$(SDK_ROOT)/CMD/services -I$(SDK_ROOT)/dialogs
LDFLAGS += -L$(SDK_LIB) -Wl,--disable-new-dtags -Wl,--no-as-needed -Wl,-rpath,'$$ORIGIN/../pteid-mw-pt/_src/eidmw/lib'
LDLIBS += -l$(PTEID_LIB) -lpteidapplayer -lpteidcardlayer -lpteidcommon -lpteiddialogsQT -lCMDServices -lpthread -ldl

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) $(LDFLAGS) $(LDLIBS) -o $@

%.o: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

run: $(TARGET)
	LD_LIBRARY_PATH="$(SDK_LIB):$$LD_LIBRARY_PATH" ./$(TARGET)