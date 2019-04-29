#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
int
main(int argc, char const *argv[])
{
        struct sockaddr_in serv_addr;
        int sock_fd ;
        int s, len ;
        char buffer[1024] = {0};
        char * data ;
	int num = atoi(argv[2]);
        FILE* fp = fopen(argv[3],"r");

        sock_fd = socket(AF_INET, SOCK_STREAM, 0) ;
        if (sock_fd <= 0) {
                perror("socket failed : ") ;
                exit(EXIT_FAILURE) ;
        }

        memset(&serv_addr, '0', sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(num);
        if (inet_pton(AF_INET,argv[1], &serv_addr.sin_addr) <= 0) {
                perror("inet_pton failed : ") ;
                exit(EXIT_FAILURE) ;
        }

        if (connect(sock_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
                perror("connect failed : ") ;
                exit(EXIT_FAILURE) ;
        }


        while(1){
                len = fread( buffer, sizeof(char), 1024, fp ) ;
                 send( sock_fd, buffer, len, 0 ) ;
                 if( feof( fp ) ) break ;
        }

        shutdown(sock_fd, SHUT_WR) ;

        char buf[1024] ;
        data = 0x0 ;
        len = 0 ;
        while ( (s = recv(sock_fd, buf, 1023, 0)) > 0 ) {
                buf[s] = 0x0 ;
                if (data == 0x0) {
                        data = strdup(buf) ;
                        len = s ;
                }
                else {
                        data = realloc(data, len + s + 1) ;
                        strncpy(data + len, buf, s) ;
                        data[len + s] = 0x0 ;
                        len += s ;
                }

        }
	 printf(">%s\n", data);

}

