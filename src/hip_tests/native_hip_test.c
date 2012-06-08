
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
	int sockfd, err, family = PF_HIP, type = SOCK_STREAM;
	char *user_priv_key = "/etc/hip/hip_host_dsa_key";	// HI
	char *hello = "hello";
	int bytes;

	struct endpoint *endpoint;
	struct sockaddr_ed my_ed;
	struct endpointinfo hints, *res = NULL;

	err = load_hip_endpoint_pem(user_priv_key, &endpoint);
	err = setmyeid(&my_ed, "", endpoint, NULL);

	sockfd = socket(family, type, 0);
	err = bind(sockfd, (struct sockaddr *) &my_ed, sizeof(my_ed));
	memset(&hints, 0, sizeof(&hints));

	hints.ei_socktype = type;
	hints.ei_family = family;
	
	// web HIP host -- crossroads.infrahip.net
	err = getendpointinfo("www.host.org", "echo", &hints, &res);
	
	bytes = send(sockfd, hello, strlen(hello), 0);
	bytes = recv(sockfd, hello, strlen(hello), 0);

	close(sockfd);
	freeaddrinfo(res);

	return 0;
}
