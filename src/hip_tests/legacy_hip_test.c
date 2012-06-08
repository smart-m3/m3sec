
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/time.h>
#include <time.h>
#include <arpa/inet.h>
#include <net/if.h>

int main(int argc,char *argv[]) {
	struct addrinfo hints, *res, *try;
	char *hello = "hello";
	int err, bytes, sock;
	
	memset(&hints, 0, sizeof(hints));
	hints.ai_flags = AI_HIP;	// problem
	hints.ai_family = AF_INET6;
	hints.ai_socktype = SOCK_STREAM;

	// web HIP host -- crossroads.infrahip.net
	err = getaddrinfo("www.host.org", "echo", &hints, &res);
	sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

	for (try = res; try; try = try->ai_next)
		err = connect(sock, try->ai_addr, try->ai_addrlen);

	bytes = send(sock, hello, strlen(hello), 0);
	bytes = recv(sock, hello, strlen(hello), 0);

	//err = close(sock);
	//err = freeaddrinfo(res);

	close(sock);
	freeaddrinfo(res);

	return 0;
}
