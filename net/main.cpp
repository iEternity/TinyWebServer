#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/epoll.h>
#include <vector>
#include <fcntl.h>
#include <errno.h>
#include <string>

#include "tinyxml2.h"

#define EVENT_LIST_SIZE 16
#define BUF_SIZE 1024

using EventLists = std::vector<epoll_event>;

void errorHandler(const char* message){
    perror(message);
    exit(1);
}

std::string readPortConfig()
{
    tinyxml2::XMLDocument doc;
    if(doc.LoadFile("ServerConfig.xml") != 0)
    {
        char* dir = getcwd(NULL, 0);
        std::cerr<<"Can't open ServerConfig.xml";
        if(dir != nullptr){
            std::cerr<<" in directory "<<dir;
        }
        std::cerr<<std::endl;
    }

    tinyxml2::XMLElement* root = doc.RootElement()->FirstChildElement();
    if(!strcmp(root->Name(), "port"))
    {
        return root->GetText();
    }
}

int main(int argc, char* argv[]) {
    std::string port = readPortConfig();

    int servSock = socket(PF_INET, SOCK_STREAM, 0);

    sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons((unsigned short)std::stoul(port));

    if(-1 == bind(servSock, (sockaddr*)&servAddr, sizeof(servAddr))){
        errorHandler("bind error");
    }

    if(-1 == listen(servSock, 5)){
        errorHandler("listen error!");
    }

    int epfd = epoll_create1(EPOLL_CLOEXEC);

    epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = servSock;

    epoll_ctl(epfd, EPOLL_CTL_ADD, servSock, &event);

    EventLists events(EVENT_LIST_SIZE);
    char buf[BUF_SIZE];

    bool quit = false;
    while(!quit){
        int numEvents = epoll_wait(epfd, &*events.begin(), static_cast<int>(events.size()), -1);
        for(int i=0;i<numEvents;i++){
            if(events[i].data.fd == servSock){
                int clientSock = accept(servSock, 0, 0);

                int flag = fcntl(clientSock, F_GETFL, 0);
                fcntl(clientSock, F_SETFL, flag|O_NONBLOCK);

                event.events = EPOLLIN|EPOLLET;
                event.data.fd = clientSock;
                epoll_ctl(epfd, EPOLL_CTL_ADD, clientSock, &event);
            }
            else{
                int sockfd = events[i].data.fd;
                while(1){
                    int readLen = read(sockfd, buf, BUF_SIZE);
                    if(readLen == -1){
                        if(errno == EAGAIN){
                            break;
                        }
                    }
                    else if(readLen == 0){
                        epoll_ctl(epfd, EPOLL_CTL_DEL, sockfd, 0);
                        close(sockfd);
                        break;
                    }
                    else{
                        write(sockfd, buf, readLen);
                    }
                }
            }
        }
    }

    close(servSock);
    close(epfd);
    return 0;
}