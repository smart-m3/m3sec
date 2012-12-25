#include "afsfuse.h"
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
//#include <unistd.h>
//#include <utime.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
//#include <sys/statfs.h>
#define N_SECURITY_OBJECTS  1

extern rxc_ExecuteRequest();
int verbose = 1;
int main(){
   struct rx_securityClass *(securityObjects[N_SECURITY_OBJECTS]);
    struct rx_service *service;
	int retcode = 0;
	char codestr[256];
	
	printf("RX Init");
	printf("Error for rxinit");
	retcode = rx_Init(htons(SAMPLE_SERVER_PORT));
    /* Initialize Rx, telling it port number this server will use for its single service */
	sprintf(codestr,"value for retcode is %d",retcode);
	printf(codestr);

    if (retcode < 0) printf("rx_init");
    /* Pass the port parameter to rx_Init() as htons(port) always */
	printf("Setting security");
    /* Create a single security object, in this case the null security object, for unauthenticated connections, which will be used to control security on connections made to this server */
    securityObjects[SAMPLE_NULL] = rxnull_NewServerSecurityObject();
    if (securityObjects[SAMPLE_NULL] == (struct rx_securityClass *) 0) printf("rxnull_NewServerSecurityObject");
	printf("Setting New Service");
    /* Instantiate a single sample service.  The rxgen-generated procedure which is called to decode requests is passed in here (TEST_ExecuteRequest). */
    service = rx_NewService(0, SAMPLE_SERVICE_ID, "sample", securityObjects, N_SECURITY_OBJECTS, rxc_ExecuteRequest);
	printf("Dead time: %d\n",service->connDeadTime);
	
	
    if (service == (struct rx_service *) 0) printf("rx_NewService");
	printf("Putting thread to pool");
    rx_StartServer(1); /* Donate this process to the server process pool */
	printf("Going to quit");
    printf("StartServer returned?");
	return 0;
}
int IsAllowedPath(char *path){
  int len = strlen(path);
   char *p = '\0';
   int i;
   int backCount = 0;
   p = path;
   for ( i=0;i<len;i++){
      switch (*p)  {
         case '/':
                 backCount++;

         case 'p':
         case 'u':
         case 'n':
         case 'e':
         case 'i':
         case 'b':
         case 'm':
         case 'c':
         case 'o':
         case 'a':
         case 'f':
         case 's':
         case '.':
                  if(backCount==3)
                     return 1;
                   p++;
                   break;
         default:
                  if(backCount<3)
                    return 0;
                    
                                    
         }
     }
     return 1;
}
    /* translate the path so that we do not show the
    "/[root]" dir contents :-)*/

void translatePath(char * path,char * lbuff ){
   strcat(lbuff,"/afs");
   strcat(lbuff+4,path);
   lbuff[strlen(lbuff)] = '\0';
}
int rxc_rxc_getattr(call,path,dummy)
 struct rx_call *call;
 char * path;
 int dummy ;
{
  struct stat stemp = {0};
  XDR xdr;
	long length;
  int error = 0,nbytes;
    char lbuff[512] ={0};
    translatePath(path,lbuff);
    printf("Server : %s Path : %s translated path: %s\n",__FUNCTION__,path, lbuff);

    length = sizeof(stemp);
if(!IsAllowedPath(lbuff))
 return -1;
        
	if(lstat(lbuff,&stemp)==-1){
	error = -errno;
  printf("eror in the server %d:\n",error);
  return error;
	}
	{
	   		
    xdrrx_create(&xdr, call, XDR_ENCODE);
  	if (!xdr_long(&xdr, &length)) error = -1;
 
    
		   if ((nbytes = rx_Write(call, (char *)&stemp, length) )!= length){
			error = -1;
		  }
		  printf("no of bytes writen%d\n",nbytes);
	}
return error;

}
int rxc_rxc_getdirWrapper(struct rx_call *z_call, char * path,bulkmydirhandles *handles)
{
	
    	DIR *dp;
    	struct dirent *de;
	int no = 0;
	my_dirhandle *t;
    char lbuff[512] ={0};
    translatePath(path,lbuff);
    printf("Server : %s Path : %s translated path: %s\n",__FUNCTION__,path, lbuff);

    dp = opendir(lbuff);
    if(dp == NULL)
        return 1;
    while((de = readdir(dp)) != NULL) {
       no++;
    }
    closedir(dp);
				
	printf("no of elements: %d\n",no);
	printf("getdirwrapper:%s",lbuff);
	handles->bulkmydirhandles_len = 0 ;
	handles->bulkmydirhandles_val = malloc(sizeof(my_dirhandle)*(no+1));
	memset(handles->bulkmydirhandles_val,0,(sizeof(my_dirhandle)*(no+1)));
	
	t = handles->bulkmydirhandles_val;
    dp = opendir(lbuff);
    if(dp == NULL)
        return 1;

	
    while((de = readdir(dp)) != NULL) {
	strcpy(t->name, de->d_name);
	printf("%s\n",t->name);
	t->type  =  de->d_type;
	t->inode = de->d_ino;
	handles->bulkmydirhandles_len ++;
	t++;
    }
    
    
    closedir(dp);
    printf("exiting getdirwrapper\n");	
    return 0;

}
 int rxc_rxc_rmdir(struct  rx_call *call, char *path)
{
    int res;
    char lbuff[512] ={0};
    translatePath(path,lbuff);
    printf("Server : %s Path : %s translated path: %s\n",__FUNCTION__,path, lbuff);

    res = rmdir(lbuff);
    if(res == -1)
        return -errno;

    return 0;
}
int rxc_rxc_write(struct  rx_call *call, char * path, afs_uint32 size, afs_uint32 offset,  struct my_file_info *fi){
	int error = 0;
	int fd;
	XDR xdr;
	int nbytes,length;
    char * buffer ;
    char lbuff[512] ={0};
    translatePath(path,lbuff);
    printf("Server : %s Path : %s translated path: %s\n",__FUNCTION__,path, lbuff);


 	if(!error){
	    	xdrrx_create(&xdr, call, XDR_DECODE);
    		if (!xdr_long(&xdr, &length)) error = 1;

      buffer = malloc(length);

		  if ((nbytes = rx_Read(call, buffer, length) ) != length){
			error = rx_Error(call);
	    }
		  printf("no of bytes recieved%d, read : %d\n",length,nbytes);
		  }

		fd = open (lbuff ,O_WRONLY);
		if (fd ==0)	{
      error = -errno;
			if(verbose)
			printf("failed to open %s\n",lbuff);
		}

    nbytes = pwrite(fd,buffer,nbytes,offset);
		if (nbytes < 0) {
				error = -errno;
				printf( "File system write failed zero data write\n");

	    	}
		if (fd >0)
			close(fd);

     free(buffer);
		return error;

}
int rxc_rxc_chmod(struct  rx_call *call, char * path,afs_uint32 mode){
    char lbuff[512] ={0};
    translatePath(path,lbuff);
    printf("Server : %s Path : %s translated path: %s\n",__FUNCTION__,path, lbuff);

    if( chmod(lbuff, mode) == -1)
        return -1;
return 0;
}
int rxc_rxc_mknod(struct  rx_call *call, char * path,afs_uint32 mode,afs_uint32 rdev){
    int res;
    char lbuff[512] ={0};
    translatePath(path,lbuff);
    printf("Server : %s Path : %s translated path: %s\n",__FUNCTION__,path, lbuff);

    res = mknod(lbuff, mode, rdev);
    if(res == -1)
        return -errno;
   return 0;
}
int rxc_rxc_chown(struct  rx_call *call, char * path,afs_uint32 uid,afs_uint32 gid){
    char lbuff[512] ={0};
    translatePath(path,lbuff);
    printf("Server : %s Path : %s translated path: %s\n",__FUNCTION__,path, lbuff);

    if( lchown(lbuff, uid,gid) == -1)
        return -1;

return 0;
}
int rxc_rxc_utime(struct  rx_call *call, char * path,afs_uint32 at,afs_uint32 mt){
    struct utimbuf buf;
    char lbuff[512] ={0};
    translatePath(path,lbuff);
    printf("Server : %s Path : %s translated path: %s\n",__FUNCTION__,path, lbuff);

   buf.actime=at;
   buf.modtime=mt;
    if(utime(lbuff, &buf) == -1)
        return -1;
    return 0;
}

int rxc_rxc_mkdir(struct  rx_call *call, char *path, mode_t mode)
{
  int res;
    char lbuff[512] ={0};
    translatePath(path,lbuff);
    printf("Server : %s Path : %s translated path: %s\n",__FUNCTION__,path, lbuff);

    res = mkdir(lbuff, mode);
    if(res == -1)
        return -errno;

    return 0;
}

 int rxc_rxc_unlink(struct  rx_call *call, char *path)
{
    int res;
    char lbuff[512] ={0};
    translatePath(path,lbuff);
    printf("Server : %s Path : %s translated path: %s\n",__FUNCTION__,path, lbuff);

    res = unlink(lbuff);
    if(res == -1)
        return -errno;

    return 0;
}

int rxc_rxc_truncate(struct  rx_call *call,  char *  path,  afs_uint32 size)   {
    int res;
    char lbuff[512] ={0};
    translatePath(path,lbuff);
    printf("Server : %s Path : %s translated path: %s\n",__FUNCTION__,path, lbuff);

    res = truncate(lbuff, size);
    if(res == -1)
        return -errno;
    return 0;
}
int rxc_rxc_statfs( struct  rx_call *call, char * path,  struct mystatfs *stbuf) {
    int res;
    struct statfs st;
    char lbuff[512] ={0};
    translatePath(path,lbuff);
    printf("Server : %s Path : %s translated path: %s\n",__FUNCTION__,path, lbuff);

    res = statfs(lbuff, &st);
    if(res == -1)
        return -errno;

  stbuf->f_type = st.f_type;
  stbuf->f_bsize = st.f_bsize;
  stbuf->f_blocks = st.f_blocks;
  stbuf->f_bfree = st.f_bfree;
  stbuf->f_bavail = st.f_bavail;
  stbuf->f_files = st.f_files;
  stbuf->f_ffree = st.f_ffree;
  stbuf->f_fsid1 = st.f_fsid.__val[0];
  stbuf->f_fsid2 = st.f_fsid.__val[1];
  stbuf->f_namelen = st.f_namelen;
  memcpy(stbuf->f_spare,st.f_spare,6 * sizeof (st.f_spare[0]));
    return 0;

}
int rxc_rxc_flush( struct  rx_call *call, char *  path,  struct my_file_info *fi){
    char lbuff[512] ={0};
    translatePath(path,lbuff);
    printf("Server : %s Path : %s translated path: %s\n",__FUNCTION__,path, lbuff);
return 0;
}
int rxc_rxc_release( struct  rx_call *call, char *  path,  struct my_file_info *fi){
    char lbuff[512] ={0};
    translatePath(path,lbuff);
    printf("Server : %s Path : %s translated path: %s\n",__FUNCTION__,path, lbuff);
    if(fi->fh)
       if(-1==close(fi->fh))
         return -errno;
         
return 0;
}
int rxc_rxc_fsync( struct  rx_call *call, char *  path, int isdatasync, struct my_file_info *fi){
    char lbuff[512] ={0};
    translatePath(path,lbuff);
    printf("Server : %s Path : %s translated path: %s\n",__FUNCTION__,path, lbuff);

return 0;
}
int rxc_rxc_readlink( struct  rx_call *call, char *  path,  afs_uint32 size, char**data)  {
    int res;
    char lbuff[512] ={0};
    translatePath(path,lbuff);
    printf("Server : %s Path : %s translated path: %s\n",__FUNCTION__,path, lbuff);

   	*data = malloc(512);
    res = readlink(lbuff, *data, 512-1);
    if(res == -1)
        return -errno;

    (*data)[res] = '\0';
    return 0;
}
 int rxc_rxc_symlink(struct  rx_call *call, char *from,  char *to)
{
    int res;
    char lbufffrom[512] ={0};
    char lbuffto[512] ={0};
    translatePath(from,lbufffrom);
    printf("Server : %s Path : %s translated path: %s\n",__FUNCTION__,from, lbufffrom);
    
    translatePath(to,lbuffto);
    printf("Server : %s Path : %s translated path: %s\n",__FUNCTION__,to, lbuffto);


    res = symlink(lbufffrom, lbuffto);
    if(res == -1)
        return -errno;

    return 0;
}

 int rxc_rxc_rename(struct  rx_call *call,char *from, char *to)
{
    int res;
    char lbufffrom[512] ={0};
    char lbuffto[512] ={0};
    translatePath(from,lbufffrom);
    printf("Server : %s Path : %s translated path: %s\n",__FUNCTION__,from, lbufffrom);

    translatePath(to,lbuffto);
    printf("Server : %s Path : %s translated path: %s\n",__FUNCTION__,to, lbuffto);


    res = rename(lbufffrom, lbuffto);
    if(res == -1)
        return -errno;

    return 0;
}

 int rxc_rxc_link(struct  rx_call *call,char *from,  char *to)
{
    int res;
    char lbufffrom[512] ={0};
    char lbuffto[512] ={0};
    translatePath(from,lbufffrom);
    printf("Server : %s Path : %s translated path: %s\n",__FUNCTION__,from, lbufffrom);

    translatePath(to,lbuffto);
    printf("Server : %s Path : %s translated path: %s\n",__FUNCTION__,to, lbuffto);


    res = link(lbufffrom, lbuffto);
    if(res == -1)
        return -errno;

    return 0;
}


int rxc_rxc_read(struct  rx_call *call, char * path, afs_uint32 size, afs_uint32 offset,  struct my_file_info *fi)
{
	int error = 0;
	int fd;
	XDR xdr;
	int nbytes;
  char * buffer = malloc(size);
  char lbuff[512] ={0};

    translatePath(path,lbuff);
    printf("Server : %s Path : %s translated path: %s\n",__FUNCTION__,path, lbuff);
		fd = open (lbuff ,O_RDONLY);
		if (fd ==0){
			if(verbose)
				printf("failed to open %s\n",lbuff);
			error =1;
		}
		if ( nbytes = pread(fd,buffer,size, offset)==-1){
		error = -errno;
		printf("server cannot seek at: %d\n", offset);
		}
    
		printf("server :no of bytes read :%d \n",nbytes);
		
		if(!error)		{
	    	xdrrx_create(&xdr, call, XDR_ENCODE);
    		if (!xdr_long(&xdr, &nbytes)) error = 1;
		   if (rx_Write(call, buffer, nbytes) != nbytes){
			error = 1;
		  }
		  printf("Server : no of bytes writen%d\n",nbytes);
		}
		if (fd >0)
			close(fd);
      
      free(buffer);
    	return error;
}

int rxc_rxc_open(struct  rx_call *z_call, char * path,  int flags, u_int *hd)
{
int error=0;
    char lbuff[512] ={0};
    translatePath(path,lbuff);
    printf("Server : %s Path : %s translated path: %s\n",__FUNCTION__,path, lbuff);
	*hd = open (lbuff ,flags);
	if(*hd == -1){
	  error = -errno;
	 }
return error;
}
