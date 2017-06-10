#include <cstdlib>
#include <cstdio>
#include <string>
#include <iostream>

//#include <stdio.h>
//#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <resolv.h>
#include <netdb.h>
//#include <stdlib.h>
#include <sys/select.h>

#define BUFLEN 16384
unsigned short DEBUG_LEVEL=0;

void usage(char *prog, char *string) {
	fprintf(stderr, "Error: %s\n", string);
	fprintf(stderr, "Usage: %s <server>\n", prog);
	exit(1);
}

void debug(unsigned short lvl, char *prog, char *string) {
	if (lvl <= DEBUG_LEVEL) {
		fprintf(stderr, "%s: %s\n", prog, string);
	}
}

char *get_ip_str(const struct sockaddr *sa, char *s, size_t maxlen) {
	switch(sa->sa_family) {
		case AF_INET:
			inet_ntop(AF_INET, &(((struct sockaddr_in *)sa)->sin_addr), s, maxlen);
			break;

		case AF_INET6:
			inet_ntop(AF_INET6, &(((struct sockaddr_in6 *)sa)->sin6_addr), s, maxlen);
			break;

		default:
			strncpy(s, "Unknown AF", maxlen);
			return s;
	}
	return s;
}

using namespace std;

int main(int argc, char *argv[]) {
	// Process arguments {{{
	if(argc != 3) {
		printf("\e[1;41m ERROR \e[0m usage: %s game-key [ipv6]:port\n", argv[0]);
		return 1;
	}
	string key(argv[1]);
	string ip(argv[2]);
	int openingBracket = ip.find('[');
	int closingBracket = ip.find(']');
	string ipAddress = ip.substr(openingBracket + 1, closingBracket - openingBracket - 1);
	int ipPort = atoi(ip.substr(closingBracket + 2).c_str());
	printf("ipv6 = (%s)\n", ipAddress.c_str());
	printf("port = (%d)\n", ipPort);
	// }}}
  // Sockets {{{
	int rval, sockfd6;
	struct addrinfo addrinfo;
	struct addrinfo *res, *r;
	struct hostent *host_ent;
	int e_save;
	int success;
	char **addrlist;
	fd_set read_fds, write_fds, except_fds;
	char buf[BUFLEN];
	char s[BUFLEN];
	int mlen;
	// }}}
	return 0;
}
