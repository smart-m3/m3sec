
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
	struct addrinfo hints, *res, *try, *ai;
	//char *hello = "hello world";
	char sentdata[1], receiveddata[1];	
	int err, bytes, sock, sentnum, receivednum;
	char i = 'a';
	
	memset(&hints, 0, sizeof(hints));

	//hints.ai_flags = AI_HIP;
	hints.ai_flags = AI_PASSIVE;	
	hints.ai_family = /*AF_INET6*/AF_UNSPEC;
	//hints.ai_family = AF_HIP;
	hints.ai_socktype = SOCK_STREAM;
	
	err = getaddrinfo("crossroads.infrahip.net", "echo", &hints, &res);

	for (try = res; try != NULL; try = try->ai_next, i++) {	

		sentdata[0] = i;
		//struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *) try->ai_addr;
		//char addr_str[INET6_ADDRSTRLEN];
		

		sock = socket(res->ai_family, res->ai_socktype, /*res->ai_protocol*/0);
		if (sock < 0) {
			sock = 0;
			printf("socket creation failed\n");
			freeaddrinfo(res);
			close(sock);
		}

		/*if (!inet_ntop(AF_INET6, (char *) &sin6->sin6_addr, addr_str, sizeof(addr_str))) {
			perror("inet_ntop\n");
			freeaddrinfo(res);
			close(sock);
		}*/

		printf("Trying to connect to crossroads.infrahip.net\n");

		if(connect(sock, try->ai_addr, try->ai_addrlen /*sizeof(struct sockaddr_in6)*/) < 0) {
			close(sock);
			sock = 0;
			printf("error: trying next\n");
			continue;
		}

		//if (err == -1) continue;
		//if (connect(sock, try->ai_addr, try->ai_addrlen) != -1) break; // success

		//close(sock);

		sentnum = send(sock, sentdata, /*strlen(hello)*/1, 0);
			if (sentnum <= 0) {
			printf("Sending failed\n");
			freeaddrinfo(res);
			close(sock);
		}

		receivednum = recv(sock, receiveddata, 1, 0);
		if (receivednum <= 0) {
			printf("Receive failed\n");
			freeaddrinfo(res);
			close(sock);	
		}

		if (!memcmp(sentdata, receiveddata, 1)) {
			printf("Data transfer OK\n");
		} else {
			printf("Data transfer failed\n");
		}

		close(sock);
	}

	return 0;
}
