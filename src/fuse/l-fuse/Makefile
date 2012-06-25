SRCDIR=./
LIBRX=${SRCDIR}lib/librx.a
LIBS=${LIBRX} ${SRCDIR}lib/liblwp.a
#CC = g++
CFLAGS=-g -I. -I${SRCDIR}include -I${SRCDIR}include/fuse/ -DDEBUG ${XCFLAGS}  -D_FILE_OFFSET_BITS=64  -DFUSE_USE_VERSION=22

all: afsfuse_client afsfuse_server

afsfuse_client: afsfuse_client.o afsfuse.xdr.o ${LIBS} bulk_io.o  afsfuse.cs.o
	${CC} ${CFLAGS} -o afsfuse_client afsfuse_client.o  ${SRCDIR}lib/fuse/fuse.o ${SRCDIR}lib/fuse/mount.o -lpthread  ${SRCDIR}lib/fuse/helper.o  ${SRCDIR}lib/fuse/fuse_mt.o bulk_io.o afsfuse.cs.o afsfuse.xdr.o ${LIBS}

afsfuse_server: afsfuse_server.o afsfuse.xdr.o afsfuse.ss.o bulk_io.o  ${LIBS}
	${CC} ${CFLAGS} -o afsfuse_server afsfuse_server.o  bulk_io.o afsfuse.ss.o afsfuse.xdr.o ${LIBS}

#afsfuse_client.o:	afsfuse.h
#afsfuse_server.o:	afsfuse.h
bulk_io.o:
	${CC} -c -g  -I${SRCDIR}include  bulk_io.c 
	 
afsfuse.cs.c afsfuse.ss.c afsfuse.er.c afsfuse.h afsfuse.xdr.c: afsfuse.xg
	rxgen afsfuse.xg
	
afsfuse.xdr.o: afsfuse.xdr.c
	${CC} -c -g -I{SRCDIR}include   afsfuse.xdr.c 
all: afsfuse_server afsfuse_client
clean:
	rm *.o

