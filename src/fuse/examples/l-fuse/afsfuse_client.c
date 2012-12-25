#include "afsfuse.h"


#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/statfs.h>
#include <netdb.h>
#include <sys/types.h>
#include<semaphore.h>
#include "fuse.h"


struct rx_connection *conn;
typedef struct rx_connection rx_connection ;
sem_t semlock;
struct rx_connection * getconnection(){
   while (sem_wait(&semlock) == -1)
      if (errno != EINTR)
         return (rx_connection * )0;
      return conn;
}
int relconnection(rx_connection *rx){
       sem_post(&semlock);
}
unsigned long GetIpAddress(hostname)
char *hostname;
{
    struct hostent *hostent;
    u_long host;
    hostent = gethostbyname(hostname);
    if (!hostent) {printf("host %s not found", hostname);exit(1);}
    if (hostent->h_length != sizeof(u_long)) {
		printf("host address is disagreeable length (%d)", hostent->h_length);
		exit(1);
    }
    bcopy(hostent->h_addr, (char *)&host, sizeof(host));
    return host;
}
int xmp_getattr(  char *path, struct stat *stbuf)
{
  struct rx_call *call;
  rx_connection *local;
  long length;
  XDR xdr;
  int nbytes;
  char mypath[512] = {0};
  int error = 0;
  strcpy(mypath,path);
  
  local = getconnection();
  call = rx_NewCall(local);
  
    if( Startrxc_rxc_getattr(call, mypath,100))
   	 goto throwerror;

    xdrrx_create(&xdr, call, XDR_DECODE);
    
    if (!xdr_long(&xdr, &length)) goto throwerror;
    
    printf("client: length of data:%d\n",length);
		nbytes = rx_Read(call, (char *)stbuf, length);
		printf("client : read rx %d bytes \n",nbytes);
    
		if (nbytes<=0){
    memset(stbuf,0,sizeof(stbuf));
    error = nbytes;
    goto throwerror;
    }
    
   rx_EndCall(call, error);
   relconnection(local);
   return 0;
throwerror:
if (error = rx_Error(call)) 
			printf("RX system read failed\n");
 rx_EndCall(call, error);
  relconnection(local);
  return error;
} 
typedef int (*td) (fuse_dirh_t h, const char *name, int type, ino_t ino);

int xmp_getdir(const char *path, fuse_dirh_t hdl, td fillerp)
{
	bulkmydirhandles  handles;
	my_dirhandle *h;
	int iter , res;
  rx_connection *local;
	memset(&handles,0,sizeof(handles));
  local = getconnection();
  
	if(rxc_rxc_getdirWrapper(local, path, &handles))
		goto throwerror;

	h = handles.bulkmydirhandles_val;
	printf("no of dirs: %d\n",handles.bulkmydirhandles_len);

    for (iter = 0; iter <= handles.bulkmydirhandles_len; iter++ ) {
	printf("name is :%s\n",h->name);
	res = fillerp(hdl, ( const char*)h->name, (int)h->type, 0);
        if(res != 0)
            break;

	 h++;
    }
    relconnection(local);
    return 0;
  throwerror:
    relconnection(local);
    return -1;  
    
}

int xmp_open( char *path, struct fuse_file_info *fi)
{

  int ret;
  rx_connection *local;  
  local = getconnection();
  ret =  rxc_rxc_open(local, path,fi->flags,&fi->fh);
  relconnection(local);
  return ret;
  
} 
int xmp_statfs(const char *path, struct statfs *stbuf)
{
   int res;
   mystatfs st;
  rx_connection *local;
  local = getconnection();  
  res =     rxc_rxc_statfs(local,path, &st);
  stbuf->f_type = st.f_type;
  stbuf->f_bsize = st.f_bsize;
  stbuf->f_blocks = st.f_blocks;
  stbuf->f_bfree = st.f_bfree;
  stbuf->f_bavail = st.f_bavail;
  stbuf->f_files = st.f_files;
  stbuf->f_ffree = st.f_ffree;
  stbuf->f_fsid.__val[0] = st.f_fsid1 ;
  stbuf->f_fsid.__val[1] = st.f_fsid2 ;
  stbuf->f_namelen = st.f_namelen;
  memcpy(stbuf->f_spare,st.f_spare,6 * sizeof (st.f_spare[0]));
  
  relconnection(local);
  return res;

}
  int xmp_read(const char *path, char *buffer, size_t size, off_t offset,struct fuse_file_info *fi)
{
  struct rx_call *call;
  int error = 0;
  struct my_file_info mfi;
  char *t;
  XDR xdr;
  int nbytes ;
  long length,readbytes=0;
  afs_uint32 msize, moffset;
  rx_connection *local;
  
  msize = size;
  moffset = offset;
  length = size;

  local = getconnection();
  call = rx_NewCall(local);

  mfi.flags = fi->flags;
  mfi.fh = fi->fh ;
  mfi.writepage = fi->writepage;

printf("xmp read\n");

if( Startrxc_rxc_read(call, path, msize, moffset, &mfi))
goto throwerror;

    xdrrx_create(&xdr, call, XDR_DECODE);
    if (!xdr_long(&xdr, &length))
     goto throwerror;
    printf("client: length of data:%d\n",length);
	t = buffer;

  while (!error || ( length != size ) ) {
		nbytes = rx_Read(call, t, 1024);
		readbytes += nbytes;
		printf("client : read rx %d bytes total now : %d\n",nbytes,readbytes);
		if (nbytes<=0)
		{
			if (rx_Error(call)) /*otherwise eof*/
			{
			printf("RX system read failed\n");
			error = 1;
			}
			break;
		}
		t += nbytes;
	}
error = rx_EndCall(call, error);
   relconnection(local);
  return readbytes;   
throwerror:
     relconnection(local);
     return 0; 
}
int xmp_write(char * path, char *buffer,size_t size, off_t offset,  struct fuse_file_info *fi){
  struct rx_call *call;
  int error = 0;
  struct my_file_info mfi;
  XDR xdr;
  int nbytes ;
  int  length;
  afs_uint32 msize, moffset;
  rx_connection *local;
  
  msize = size;
  moffset = offset;
  length = size;
  
  local = getconnection();  
  call = rx_NewCall(local);

  mfi.flags = fi->flags;
  mfi.fh = fi->fh ;
  mfi.writepage = fi->writepage;

printf("xmp read\n");

if( Startrxc_rxc_write(call, path, msize, moffset, &mfi))
	goto throwerror;

    xdrrx_create(&xdr, call, XDR_ENCODE);
    if (!xdr_long(&xdr, &length))
    	goto throwerror;

    printf("client: length of data:%d\n",length);
     nbytes = rx_Write(call, buffer, length);

     printf("client : written rx %d bytes total now : %d\n",nbytes,length);
     if (nbytes<=0) 
       goto throwerror;

error = rx_EndCall(call, error);

  if (error)
  goto throwerror;

  relconnection(local);
  return length;

throwerror:
			if (error = rx_Error(call)) 
			printf("RX system read failed\n");
      relconnection(local);
return error;

}
  int xmp_chmod(const char *path, mode_t mode)
{
  rx_connection *local;
  int ret;
  local = getconnection();
ret = rxc_rxc_chmod(local, path,mode);
  relconnection(local);
  return ret;
}

  int xmp_chown(const char *path, uid_t uid, gid_t gid)
{
  rx_connection *local;
  int ret;
  local = getconnection();
  ret = rxc_rxc_chown(local, path,uid,gid);
  relconnection(local);
  return ret;
}

 int xmp_utime(const char *path, struct utimbuf *buf)
{

  rx_connection *local;
  int ret;
  local = getconnection();
  ret = rxc_rxc_utime(local, path,buf->actime,buf->modtime);
  relconnection(local);
  return ret;
}
int xmp_mknod(const char *path, mode_t mode, dev_t rdev)
{
  rx_connection *local;
  int ret;
  local = getconnection();
  ret = rxc_rxc_mknod(local,path,( afs_uint32)mode,( afs_uint32) rdev);
  relconnection(local);
  return ret;
}
  int xmp_readlink(const char *path, char *buf, size_t size)
{
  rx_connection *local;
  int ret;
  char *buffer = malloc (512);
  memset(buffer,0,512);  
  memset(buf,0,size);
  local = getconnection();
  ret = rxc_rxc_readlink(local,path,512,&buffer);
  relconnection(local);
  strncpy(buf,buffer,512-1);
  return ret;
}

  int xmp_mkdir(const char *path, mode_t mode)
{
  rx_connection *local;
  int ret;
  local = getconnection();
  ret = rxc_rxc_mkdir(local, path, mode);
  relconnection(local);
  return ret;
}


  int xmp_unlink(const char *path)
{
  rx_connection *local;
  int ret;
  local = getconnection();
  ret = rxc_rxc_unlink(local, path);
  relconnection(local);
  return ret;
}
  int xmp_rmdir(const char *path)
{
  rx_connection *local;
  int ret;
  local = getconnection();
  ret = rxc_rxc_rmdir(local,path);
  relconnection(local);
  return ret;
}

  int xmp_symlink(const char *from, const char *to)
{
  rx_connection *local;
  int ret;
  local = getconnection();
  ret = rxc_rxc_symlink(local,from, to);
  relconnection(local);
  return ret;
}

  int xmp_rename(const char *from, const char *to)
{
  rx_connection *local;
  int ret;
  local = getconnection();
  ret = rxc_rxc_rename(local,from, to);
  relconnection(local);
  return ret;
}
  int xmp_link(const char *from, const char *to)
{
  rx_connection *local;
  int ret;
  local = getconnection();
  ret = rxc_rxc_link(local, from, to);
  relconnection(local);
  return ret;
}
  int xmp_truncate(const char *path, off_t size)
{
  rx_connection *local;
  int ret;
  local = getconnection();
  ret = rxc_rxc_truncate(local, path, size);
  relconnection(local);
  return ret;
}
  int xmp_release(const char *path, struct fuse_file_info *fi)
{
 my_file_info mfi;
 /*mfi.flags = fi->flags;
 mfi.fh = fi->fh ;
 mfi.writepage = fi->writepage;*/
  rx_connection *local;
  int ret;
  local = getconnection();
  ret = rxc_rxc_release(local,path, &mfi);
  relconnection(local);
  return ret;
}
  int xmp_fsync(const char *path, int isdatasync,
                   struct fuse_file_info *fi)
{
 my_file_info mfi;
/* mfi.flags = fi->flags;
 mfi.fh = fi->fh ;
 mfi.writepage = fi->writepage;*/
   rx_connection *local;
  int ret;
  local = getconnection();
  ret = rxc_rxc_fsync(local,path,isdatasync, &mfi);
  relconnection(local);
  return ret;
}
 int xmp_flush (const char *path, struct fuse_file_info *fi){
 my_file_info mfi;

/* mfi.flags = fi->flags;
 mfi.fh = fi->fh ;
 mfi.writepage = fi->writepage;*/
   rx_connection *local;
  int ret;
  local = getconnection();
  ret = rxc_rxc_flush(local,path,&mfi);
  relconnection(local);
  return ret;
 }

  struct fuse_operations xmp_oper = {
    .getattr	= xmp_getattr,
    .readlink	= xmp_readlink,
    .getdir	= xmp_getdir,
    .mknod	= xmp_mknod,
    .mkdir	= xmp_mkdir,
    .symlink	= xmp_symlink,
    .unlink	= xmp_unlink,
    .rmdir	= xmp_rmdir,
    .rename	= xmp_rename,
    .link	= xmp_link,
    .chmod	= xmp_chmod,
    .chown	= xmp_chown,
    .truncate	= xmp_truncate,
    .utime	= xmp_utime,
    .open	= xmp_open,
    .read	= xmp_read,
    .write	= xmp_write,
    .statfs	= xmp_statfs,
    .release	= xmp_release,
    .fsync	= xmp_fsync,
    .flush = xmp_flush
    
};


int main(int argc, char *argv[]){
    unsigned long host;
    char  server [128] = {0};
    struct rx_securityClass *null_securityObject;
    if (sem_init(&semlock, 0, 1) == -1) {
      perror("Failed to initialize semaphore");
      return 1;
   }
    rx_Init(0);
    printf("Please Enter the name of the server default: localhost.\n");
    scanf("%s",server);
    if (strlen(server)==0)
    host = GetIpAddress("127.0.0.1");
    else
    host = GetIpAddress(server);    
    null_securityObject = rxnull_NewClientSecurityObject();
    conn = rx_NewConnection(host, htons(SAMPLE_SERVER_PORT), SAMPLE_SERVICE_ID, null_securityObject, SAMPLE_NULL);

    return fuse_main(argc, argv, &xmp_oper);
}
