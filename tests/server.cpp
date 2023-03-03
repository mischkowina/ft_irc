#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <arpa/inet.h>

void error(const char *msg) {
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[]) {

    char buffer[256];
    int sockfd, portno, n;
    struct sockaddr_in serv_addr, cli_addr;
    struct hostent *server;
    socklen_t clilen;

    if (argc < 3) {
        std::cerr << "usage " << argv[0] << " hostname port" << std::endl;
        exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        std::cerr << "ERROR, no such host" << std::endl;
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
        error("ERROR on binding");

    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    int newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0) 
        error("ERROR on accept");

    char ipstr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(cli_addr.sin_addr), ipstr, INET_ADDRSTRLEN);
    std::cout << "Server IP address: " << ipstr << std::endl;

    while (true) {
        bzero(buffer,256);
        n = read(newsockfd,buffer,255);
        if (n < 0) 
            error("ERROR reading from socket");
        std::cout << "Here is the message: " << buffer << std::endl;
        n = write(newsockfd,"I got your message",18);
        if (n < 0) 
            error("ERROR writing to socket");
    }

    close(newsockfd);
    close(sockfd);
    return 0;
}

