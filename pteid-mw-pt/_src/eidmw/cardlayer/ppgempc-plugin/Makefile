OUTPUT = libpteidpp2-gempc.so 
SOURCES = pinpad-gempc-plugin.c
LDFLAGS = -lpcsclite
TARGET = /usr/local/lib/pteidpp

all:
	$(CC) -I/usr/include/PCSC/ -O2 -shared -fPIC -o $(OUTPUT) $(SOURCES)

clean:
	rm -fv *.o *.so

install:
	mkdir -p $(TARGET)
	install -m 644 $(OUTPUT) $(TARGET)


