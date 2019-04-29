#include <wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

void
child_proc(int conn)
{
        struct sockaddr_in serv_addr;
        int sock_fd ;
        char buf[1024] ;
        char * data = 0x0, * orig = 0x0 ;
        int len = 0 ;
        int length;
        int s ;
        int status;
        FILE* fp = fopen("work.c","w");
        FILE* fp2 = fopen("result.txt","w+");
        char buff[1024];
        char *dir_gcc = "/usr/bin/gcc";
        char *cmd_gcc[] = {"gcc", "-o", "work","work.c", NULL};
        int ffd;
         printf("sucess!\n");
        if((ffd = open("result.txt", O_RDWR | O_CREAT))==-1){ /*open the file */
                 perror("open");
         exit(-1);
        }

        dup2(ffd,STDOUT_FILENO); /*copy the file descriptor fd into standard output*/
        dup2(ffd,STDERR_FILENO);
        close(ffd);

        while ( (s = recv(conn, buf, 1024, 0)) > 0 ) {
                fwrite(buf,sizeof(char),s,fp);
                buf[s] =0x0;
        }

        fclose(fp);

        if(fork()==0){
                if(fork()==0){
                        execvp(dir_gcc,cmd_gcc);
                }
                else{   wait(&status);
                        execl("./work","work",NULL);
                        return;
                }
        }
        else{
                wait(&status);
        }

        fseek(fp2, 0, SEEK_END);
        length = ftell(fp2);
        rewind(fp2);

         while(1){
                len = fread( buff, sizeof(char), 1024, fp2 ) ;
	send( conn, buff, len, 0 ) ;
                 if( feof(fp2) ) break ;
        }
        fclose(fp2);
        printf("%s",buff);
        shutdown(conn, SHUT_WR);

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
        address.sin_port = htons(8090);
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
                        break;
                }
                else {
                        close(new_socket) ;
                }

        }
}

