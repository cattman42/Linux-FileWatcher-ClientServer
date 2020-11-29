                /* LEFT OFF ~64, WORKING ON SETTING UP SERVER DIRECTORY 
                +++++++++++++++++++++++++++++++++++++++++++++++++++++++
                ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                */
#include "csapp.h"
#include "sys/types.h"
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char **argv) 
{
    int listenfd, connfd, filecheck;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;  /* Enough space for any address */  //line:netp:echoserveri:sockaddrstorage
    char client_hostname[MAXLINE], client_port[MAXLINE], buf[MAXLINE], buffer[32];
    rio_t rio;
    size_t n;
    DIR* DIR;
    struct dirent* entp;
    struct stat s = {0};
    char path[1024];
    char fileName[100], fileSize[100];

    if (argc != 2) {
    argv[1] = "8000"; //This is hard set because my launch.json wouldn't recognize csapp.c
    }
    listenfd = Open_listenfd(argv[1]);
    while (1) {
	clientlen = sizeof(struct sockaddr_storage); 
	connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
        Getnameinfo((SA *) &clientaddr, clientlen, client_hostname, MAXLINE, 
                    client_port, MAXLINE, 0);
        printf("Connected to (%s, %s)\n", client_hostname, client_port);
    // Do things here
    if(stat("./Managed", &s) == -1){
        mkdir("./Managed", 0700);
    }
    DIR = opendir("./Managed");
    Rio_readinitb(&rio, connfd);
        Rio_readlineb(&rio, buf, MAXLINE); //Read request
	    if(strcmp(buf, "upload\n") == 0) {
            printf("Received: %s", buf);
            Rio_readlineb(&rio, fileName, MAXLINE); // Read filename
            strtok(fileName, "\n");
            printf("Filename: %s\n", fileName);
            Rio_readlineb(&rio, fileSize, MAXLINE); // Read fileSize
            printf("Filesize: %s", fileSize);
            size_t size = (size_t)atoi(fileSize);
            char* pointer = malloc(size);
            Rio_readnb(&rio, pointer, size); // Read file
            int srcfd = Open(fileName, O_WRONLY|O_CREAT|O_TRUNC, S_IRWXU|S_IRWXG|S_IRWXO);
            free(pointer);
            printf("Received file\n");
            Rio_writen(connfd, "Processing\n", strlen("Processing\n")); // Write processing
        }
        else if(strcmp(buf, "status\n") == 0) {
            printf("Received: %s", buf);
            Rio_readlineb(&rio, buf, MAXLINE); // Read filename
            printf("Filename: %s", buf);
            strtok(fileName, "\n");
            Rio_writen(connfd, "Processing\n", strlen("Processing\n")); // Write processing
            printf("Sent processing\n");
            filecheck = 0;
            while((entp = readdir(DIR))!=NULL) {
                strcpy(path,argv[1]);
                strcat(path,"/");
                strcat(path,entp->d_name);
                stat(path,&s);
                /* Only pay attention to files, and ignore directories. */
                if(strcmp(entp->d_name, fileName) == 0) {
                    printf("%s: %ld\n",entp->d_name,s.st_mtime);
                    printf("Sucess!\n");
                    sprintf(buffer, "%ld\n", s.st_mtime);
                    Rio_writen(connfd, buffer, strlen(buffer)); // Write mod time
                    filecheck = 1;
                }
            }
            if(filecheck == 0) {
                Rio_writen(connfd, "0\n", strlen("0\n")); // Write mod time 0
            }
        }
        else{printf("Unknown signal received.\n");}
	    Close(connfd);
        printf("Closing client \n\n");
    }
    exit(0);
}
/* $end echoserverimain */
