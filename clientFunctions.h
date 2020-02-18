//
// Created by most on 16.12.2018.
//

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <time.h>
#include <arpa/inet.h>
#include <iostream>
#include <string>
#include <string.h>

using namespace std;

#ifndef CLION_CLIENTFUNCTIONS_H
#define CLION_CLIENTFUNCTIONS_H


class clientFunctions {
public:
    void String_Write(const string &input, int sd);
    void String_Read(string &output, int sd);
    void login(int sd);
    void inregistrare(int sd);
    void exit();
    void platforma(int sd);
    void SchimbReplici(int sd);
    void IstoricMesaje(int sd, string &destinatar);
    bool StartConversatie(int sd);
    void ConversatieNouaClient(int sd);
    bool AfiseazaIstoric(int sd);
    bool Reply(int sd);
    bool StartConversatieGrup(int sd);
    void SchimbRepliciGrup(int sd);
    void AfiseazaIstoricGrup(int sd);
    void ReplyGrup(int sd);
};


#endif //CLION_CLIENTFUNCTIONS_H
