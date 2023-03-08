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

    char	buffer[256];
    int		sockfd, portno, n;
    struct	sockaddr_in serv_addr;
    struct	hostent *server;

    if (argc < 3) {
        std::cerr << "usage " << argv[0] << " hostname port" << std::endl;
        exit(0);
    }

    // Get server hostname and port number
    char* hostname = argv[1];
    portno = atoi(argv[2]);

    // Resolve server hostname to IP address
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo(hostname, NULL, &hints, &res);
    if (status != 0) {
        std::cerr << "ERROR, cannot resolve hostname" << std::endl;
        exit(0);
    }

    struct in_addr server_ip = ((struct sockaddr_in *) res->ai_addr)->sin_addr;
    std::cout << "Server IP address: " << inet_ntoa(server_ip) << std::endl;

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    // Connect to server
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);
    serv_addr.sin_addr = ((struct sockaddr_in *) res->ai_addr)->sin_addr;
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");

    // Print client IP address
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    getsockname(sockfd, (struct sockaddr *)&client_addr, &client_addr_len);
    std::cout << "Client IP address: " << inet_ntoa(client_addr.sin_addr) << std::endl;

    // Send and receive messages
    while (true) {
        std::cout << "Please enter the message: ";
        bzero(buffer,256);
        std::cin.getline(buffer,255);
        n = write(sockfd,buffer,strlen(buffer));
        if (n < 0) 
             error("ERROR writing to socket");
        bzero(buffer,256);
        n = read(sockfd,buffer,255);
        if (n < 0) 
             error("ERROR reading from socket");
        std::cout << buffer << std::endl;
    }

    close(sockfd);
    return 0;
}

