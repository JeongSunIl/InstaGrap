#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

void
child_proc(int conn)
{
        struct sockaddr_in serv_addr;
        int sock_fd ;
        char buf[1024] ;
        char * data = 0x0, * orig = 0x0 ;
        int len = 0 ;
        int s ;
        FILE* fp = fopen("recv.txt","w");
        char buff[1024];
	char rbuf[1024];
	char cor[16] = "Correct!";
 	char fai[16] = "Fail!";
        FILE* fp2 = fopen("./testcase/1.out.txt","r");
        sock_fd = socket(AF_INET, SOCK_STREAM, 0) ;
        if (sock_fd <= 0) {
                perror("socket failed : ") ;
                exit(EXIT_FAILURE) ;
        }
        
        memset(&serv_addr, '0', sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(8112);
        if (inet_pton(AF_INET,"127.0.0.1", &serv_addr.sin_addr) <= 0) {
                perror("inet_pton failed : ") ;
                exit(EXIT_FAILURE) ;
        }

        if (connect(sock_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
                perror("connect failed : ") ;
                exit(EXIT_FAILURE) ;
        }

        printf("connecting...\n");
        while ( (s = recv(conn, buf, 1024, 0)) > 0 ) {
                buf[s] =0x0;       
                send(sock_fd, buf, s, 0 ) ;
        }
        printf("%s\n",buf);

        shutdown(conn, SHUT_WR);
        shutdown(sock_fd,SHUT_WR); 
        while ( (len = recv(sock_fd, buff, 1023, 0)) > 0 ) {
               fwrite(buff, sizeof(char), len, fp);
                buff[len] = 0x0;
                }
        fclose(fp);

	fseek(fp2, 0, SEEK_END);
        int length;
        rewind(fp2);

        while(1){
                length = fread( rbuf, sizeof(char), 1024, fp2 ) ;
                 if( feof( fp2 ) ) break ;
        }
        fclose(fp2);
        if(!strcmp(buff,rbuf)){
                send(conn, cor,16,0);
        }
        else{
            send(conn, fai,16,0);
        }
}

       

int 
main(int argc, char const *argv[]) 
{ 
        int listen_fd, new_socket ; 
struct sockaddr_in address;
        int opt = 1;
        int addrlen = sizeof(address);

        char buffer[1024] = {0};

        listen_fd = socket(AF_INET /*IPv4*/, SOCK_STREAM /*TCP*/, 0 /*IP*/) ;
        if (listen_fd == 0)  {
                perror("socket failed : ");
                exit(EXIT_FAILURE);
        }

        memset(&address, '0', sizeof(address));
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY /* the localhost*/ ;
        address.sin_port = htons(8111);
        if (bind(listen_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
                perror("bind failed : ");
                exit(EXIT_FAILURE);
        }

        while (1) {
                if (listen(listen_fd, 16 /* the size of waiting queue*/) < 0) {
                        perror("listen failed : ");
                        exit(EXIT_FAILURE);
                }

                new_socket = accept(listen_fd, (struct sockaddr *) &address, (socklen_t*)&addrlen) ;
                if (new_socket < 0) {
                        perror("accept");
                        exit(EXIT_FAILURE);
                }

                if (fork() >0) {
                        child_proc(new_socket) ;
                }
                else {
                        close(new_socket) ;
                }
        }
}

