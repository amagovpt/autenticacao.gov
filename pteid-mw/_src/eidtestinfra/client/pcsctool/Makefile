OBJECTS = main.o pcscerr2string.o utils.o

TARGET = pcsctool

CPPFLAGS = -Wall -O3 #$(ARCHS) -fmessage-length=0
INCLUDE = -I. -I/usr/include/PCSC

all: $(TARGET)

clean:
	- rm *.o $(TARGET)

$(TARGET): $(OBJECTS)
	g++ $(OBJECTS) -lpcsclite -o $(TARGET)

%.o: %.cpp *.h
	g++ -c $(INCLUDE) $(CPPFLAGS) $< -o $@
