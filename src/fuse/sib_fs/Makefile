sibfs: main.o db.o
	g++ main.o db.o -Wall -D_FILE_OFFSET_BITS=64 -lsqlite3 -o sibfs

main.o: main.cpp
	g++ main.cpp `pkg-config --cflags --libs fuse` -DFUSE_USE_VERSION=27

db.o: db.cpp db.h
	g++ db.cpp

clean:
	rm -rf *o sibfs

#g++ hello.cpp `pkg-config --cflags --libs fuse` -DFUSE_USE_VERSION=27 -o hellofs
