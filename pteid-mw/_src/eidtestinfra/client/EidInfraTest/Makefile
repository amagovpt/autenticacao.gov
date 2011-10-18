TARGET = EidInfraTest

OBJS1 = ../EidTestinfraLib/Logger.o ../EidTestinfraLib/controlFileAccess.o \
	../EidTestinfraLib/virtualCardAccess.o ../EidTestinfraLib/Mutex.o \
	../EidTestinfraLib/util.o ../EidTestinfraLib/xml.o
OBJS2 =	../pcscproxy/APDURequest.o ../pcscproxy/FileStructure.o \
	../pcscproxy/SoftReader.o ../pcscproxy/WinscardImpl.o \
	../pcscproxy/Carddata.o ../pcscproxy/HexUtils.o \
	../pcscproxy/SoftReaderManager.o ../pcscproxy/proxy_pcsclite.o \
	../pcscproxy/Control.o ../pcscproxy/SoftCard.o ../pcscproxy/Transaction.o
OBJS3 = ../pcsctool/utils.o
OBJS4 = CommandTest.o ControlTest.o FileStructureTest.o utils2.o \
	commandtestfuctions.o SoftCardTest.o VirtualCardTest.o \
	ControlFileTest.o EidInfraTest.o SoftReaderManagerTest.o XMLTest.o

CPPFLAGS = -Wall -O3
INCLUDES = -I../EidTestinfraLib -I../pcscproxy -I../pcsctool -I/usr/include/PCSC \
	-I../../ThirdParty/unittest-cpp-1.3/UnitTest++/src/
LIBS = ../EidTestinfraLib/Release/libEidTestinfra.a  -ldl -lpthread \
	../../ThirdParty/unittest-cpp-1.3/UnitTest++/libUnitTest++.a
CC = g++

all: $(TARGET)

clean:
	- rm $(OBJS1) $(OBJS2) $(OBJS3) $(OBJS4)

$(TARGET): $(OBJS1) $(OBJS2) $(OBJS3) $(OBJS4)
	$(CC) $(OBJS1) $(OBJS2) $(OBJS3) $(OBJS4) $(LIBS) -o $(TARGET)

%.o: %.cpp *.h
	$(CC) -c $(CPPFLAGS) $(INCLUDES) $< -o $@

../EidTestinfraLib/%.o: ../EidTestinfraLib/%.cpp ../EidTestinfraLib/*.h
	$(CC) -c $(CPPFLAGS) $(INCLUDES) $< -o $@
 
../pcscproxy/%.o: ../pcscproxy/%.cpp ../pcscproxy/*.h
	$(CC) -c $(CPPFLAGS) $(INCLUDES) $< -o $@

../pcsctool/%.o: ../pcsctool/%.cpp ../pcsctool/*.h
	$(CC) -c $(CPPFLAGS) $(INCLUDES) $< -o $@

