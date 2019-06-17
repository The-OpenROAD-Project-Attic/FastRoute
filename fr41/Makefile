CPP=g++
CFLAGS = -O3 -I.

SRC     = dist.cpp dl.cpp err.cpp heap.cpp mst2.cpp neighbors.cpp \
	bookshelf_IO.cpp memAlloc.cpp flute.cpp flute_mst.cpp \
	DataProc.cpp RSMT.cpp route.cpp maze.cpp EdgeShift.cpp RipUp.cpp \
	utility.cpp maze3D.cpp
OBJ     = $(SRC:.cpp=.o)

cr: main.cpp ${OBJ}  
	$(CPP) $(CFLAGS) -o FastRoute main.cpp ${OBJ} -lm

#flute-net: flute-net.cpp ${OBJ}
#	$(CPP) $(CFLAGS) -o flute-net flute-net.cpp ${OBJ} -lm

#flute-ckt: flute-ckt.cpp ${OBJ}
#	$(CPP) $(CFLAGS) -o flute-ckt flute-ckt.cpp ${OBJ} -lm

rand-pts: rand-pts.cpp
	$(CPP) $(CFLAGS) -o rand-pts rand-pts.cpp 

flute.o: flute.cpp flute.h
	$(CPP) $(CFLAGS) -c -o flute.o flute.cpp

flute_mst.o: flute_mst.cpp flute.h
	$(CPP) $(CFLAGS) -c -o flute_mst.o flute_mst.cpp

DataProc.o: DataProc.cpp DataType.h DataProc.h
	$(CPP)  $(CFLAGS) -c -o DataProc.o DataProc.cpp

RSMT.o: RSMT.cpp RSMT.h DataType.h DataProc.h flute.h
	$(CPP)  $(CFLAGS) -c -o RSMT.o RSMT.cpp

route.o: route.cpp route.h DataType.h DataProc.h flute.h
	$(CPP)  $(CFLAGS) -c -o route.o route.cpp

maze.o: maze.cpp maze.h DataType.h DataProc.h
	$(CPP)  $(CFLAGS) -c -o maze.o maze.cpp

EdgeShift.o: EdgeShift.cpp EdgeShift.h DataType.h DataProc.h flute.h RipUp.h
	$(CPP)  $(CFLAGS) -c -o EdgeShift.o EdgeShift.cpp

RipUp.o: RipUp.cpp RipUp.h DataType.h flute.h DataProc.h
	$(CPP)  $(CFLAGS) -c -o RipUp.o RipUp.cpp

utility.o: utility.cpp utility.h DataType.h DataProc.h
	$(CPP)  $(CFLAGS) -c -o utility.o utility.cpp

maze3D.o: maze3D.cpp maze3D.h DataType.h DataProc.h
	$(CPP)  $(CFLAGS) -c -o maze3D.o maze3D.cpp







clean:
	rm -f *.o core*

