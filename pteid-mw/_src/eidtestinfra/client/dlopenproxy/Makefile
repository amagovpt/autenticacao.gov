TARGET=libdl_proxy.so.1.0

all: $(TARGET)

clean:
	-rm  $(TARGET) *.o

$(TARGET): dl_proxy.c
	gcc -fPIC -c dl_proxy.c -o dl_proxy.o
	gcc -shared -o  $(TARGET) dl_proxy.o -ldl
