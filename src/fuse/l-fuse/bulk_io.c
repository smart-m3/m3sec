#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <rx/rx.h>
#include <rx/xdr.h>


int bulk_SendFile(fd, call)
FILE* fd;
register struct rx_call *call;
{
    char buffer [1024] ;
    int blockSize,nbytes;
    long length = 1024;
    XDR xdr;
    long error = 0;
    
	
    xdrrx_create(&xdr, call, XDR_ENCODE);
    if (!xdr_long(&xdr, &length)) error = 1;
	
    while (!error ) 
	{
		
		nbytes = fread(buffer,sizeof (char),1024, fd);
		printf("no of bytes read%d\n",nbytes);
		if (nbytes <= 0) 
		{
			
			if (ferror(fd)) /*otherwise eof*/
			{ 
				printf( "File system read failed\n");
				error = 1;
			}
			 break;
}
		
		{
			if (rx_Write(call, buffer, nbytes) != nbytes){
				printf("no of bytes writen%d\n",nbytes);
				error = 1;
				break;
			}
		}
		
		
    }
	
    return error;
}

/* Copy the appropriate number of bytes from the call to fd.  The status should reflect the file's status coming into the routine and will reflect it going out of the routine, in the absence of errors */
int bulk_ReceiveFile(fd, call)
FILE* fd;
register struct rx_call *call;
{
    char buffer [1024];
    long length ;
    XDR xdr;
	int nbytes ;
	
    long error = 0;
	
    xdrrx_create(&xdr, call, XDR_DECODE);
    if (!xdr_long(&xdr, &length)) return 1;
	
    while (!error ) {
		nbytes = rx_Read(call, buffer, 1024);
		if (nbytes<=0) 
		{
			if (rx_Error(call)) /*otherwise eof*/
			{
			printf("RX system read failed\n");
			error = 1;
			}
			break;
		}
		if (fwrite(buffer,sizeof(char),nbytes, fd) != nbytes) {
			printf("File system write failed!\n");
			error = 1;
			break;
		}
    }
    return error;
}
