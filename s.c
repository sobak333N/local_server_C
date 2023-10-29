#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <error.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <math.h>

#define errExit(msg)    do { perror(msg); _exit(EXIT_FAILURE); } while (0)

#define BUF 1024

#define FIN "exit"
#define GET "GET"
#define DATA "Answer\n"
#include "data.h"


void child_handler(int sig) {
    pid_t pid;
    while((pid = waitpid(-1, NULL, WNOHANG)) > 0);
}


void session(int msgsock){     
   int sock;
   socklen_t length;
   struct sockaddr_in server;
   char ibuf[BUF];
   char obuf[BUF];
   int rval,sval;
   const int enable = 1; 


   do {
      memset(ibuf, 0, BUF);
      if ((rval = read(msgsock, ibuf, 1024)) == -1)
         perror("reading stream message");
      dprintf(1,"-[%d]\n",rval);
      if (rval == 0)
         printf("Connection finished\n");
      else{
         dprintf(1,"[%s]\n",ibuf);
         dprintf(1,"[%c]\n",ibuf[5]);

         struct stat filestat;
         int f;



         if(ibuf[5] == '1'){
            if ((f = open("1", O_RDONLY)) == -1) {
            perror("open: ");
            exit(errno);
            }
         }
         if(ibuf[5] == '2'){
            if ((f = open("2", O_RDONLY)) == -1) {
            perror("open: ");
            exit(errno);
            }
         }
         if(ibuf[5] == '3'){
            if ((f = open("3", O_RDONLY)) == -1) {
            perror("open: ");
            exit(errno);
            }
         }




         fstat(f , &filestat);

         int size = filestat.st_size;


         char* try;
         try = (char *)mmap(0,size , PROT_READ , MAP_SHARED,f,0);
         // printf("%s ",try);
         // printf("\n");
         // printf("%s", DATA1);


         char* result = malloc(sizeof(DATA1) + strlen(try)+1);

         // printf("%d ",strlen(DATA1));

         memcpy(result, DATA1, sizeof(DATA1));
         memcpy(result+strlen(DATA1) , try, strlen(try)); 

         printf("%s ",result);


         memset(obuf, 0, BUF);
         if (!strncmp(ibuf,GET,3)){
            strcpy(obuf,result);
         }else{
            strcpy(obuf,DATA);
         } 
         sval=send(msgsock,obuf,strlen(obuf),0);
         dprintf(1,"Sent [%d]\n",sval);
      }
   } while ((rval > 0) && (strncmp(ibuf,GET,3)));



   exit(0);
}


int main(int argc, char *argv[]) {
   int sock;
   socklen_t length;
   struct sockaddr_in server;
   int msgsock;
   char ibuf[BUF];
   char obuf[BUF];
   int rval,sval;
   const int enable = 1; 


   struct sigaction new_act, old_act;
   sigemptyset (&new_act.sa_mask);
   new_act.sa_flags = SA_RESTART;
   new_act.sa_handler = child_handler;
   sigaction (SIGCHLD, &new_act, &old_act);


   // int f;
   // if ((f = open("index", O_RDONLY)) == -1) {
   //    perror("open: ");
   //    exit(errno);
   //  }


   // fstat(f , &filestat);

   //  int size = filestat.st_size;


   // char* try;
   // try = (char *)mmap(0,size , PROT_READ , MAP_SHARED,f,0);
   // printf("%s ",try);
   // printf("\n");
   // printf("%s", DATA1);


   // char* result = malloc(sizeof(DATA1) + strlen(try)+1);

   // // printf("%d ",strlen(DATA1));

   // memcpy(result, DATA1, sizeof(DATA1));
   // memcpy(result+strlen(DATA1) , try, strlen(try)); 

   // printf("%s", result); 
   // exit(0);





   sock = socket(AF_INET, SOCK_STREAM, 0);
   if (sock == -1) {
      perror("opening stream socket");
      exit(1);
   }
   if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) exit(2);
   server.sin_family = AF_INET;
   server.sin_addr.s_addr = htonl(INADDR_ANY);
   if(argc == 2) server.sin_port = htons(atoi(argv[1]));
   else server.sin_port = 0;
   
   if (bind(sock, (struct sockaddr *) &server, sizeof server)  == -1) {
      perror("binding stream socket");
      exit(1);
   }

   length = sizeof server;
   if (getsockname(sock,(struct sockaddr *) &server, &length) == -1) {
      perror("getting socket name");
      exit(1);
   }
   printf("Socket port %d\n", ntohs(server.sin_port));

   listen(sock, 5);
   pid_t cpid;



   do {
      if ((msgsock = accept(sock,(struct sockaddr *) NULL,(socklen_t *) NULL)) == -1) 
         perror("accept");
      else{
         cpid = fork();
         if(cpid == 0){
            session(msgsock);
         }
      } 

      close(msgsock);
   } while(strcmp(ibuf,FIN));



   exit(0);
}

