//
// Created by most on 16.12.2018.
//

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>
#include <iostream>
#include <string>
#include <json.hpp>
#include "nlohmannn/json.hpp"
#include "fstream"
#include <vector>

using namespace std;
using json=nlohmann::json;


#ifndef CLION_SERVERFUNCTIONS_H
#define CLION_SERVERFUNCTIONS_H


class serverFunctions {
public:
    void Format_Write(string &input, int fileDescriptor);
    void Format_Read(string &output, int fileDescriptor);
    char* conversie_adresa(struct sockaddr_in adresa);
    void Login(int fileDescriptor);
    void inregistrare(int fileDescriptor);
    void quit(int fileDescriptor, fd_set &activefds);
    string verificareUsername(string usr);
    bool verifJson(string username);
    void platforma(int fileDescriptor,fd_set &activefds);
    string utilizatoriInregistrati();
    string printUtilizatoriInregistrati();
    string getIDusr(string &username);
    void SchimbRepliciServer(int fileDescriptor, string &destinatar);
    void UpdateConversatie(string &mesaj, string &destinatar);
    void IstoricMesajeServer(int fileDescriptor, string &username);
    bool StartConversatieServer(int fileDescriptor);
    bool VerifConversatie(int fileDescriptor, string &destinatar);
    void CreareConversatie(string &destinatar);
    string RefreshConversatie(string &destinatar,int &nrmsg);
    string IstoricConversatie(string &destinatar);
    void ConversatieNouaServer(int fileDescriptor,string & destinatar);
    bool AfiseazaIstoricServer(int fileDescriptor);
    void ReplyMesajServer(int &pozitie, string &mesaj, string &destinatar);
    bool ReplyServer(int fileDescriptor, string &destinatar);
    bool VerifPoz(string &destinatar, int &pozitie);
    bool StartConversatieGrupServer(int fileDescriptor);
    void SchimbRepliciGrupServer(int fileDescriptor);
    bool VerifConvGrup();
    void CreareConvGrup();
    void UpdateConvGrup(string &mesaj);
    string RefreshConvGrup(int &nrmsg);
    void AfiseazaIstoricGrupServ(int fileDescriptor);
    string IstoricMesajeGrupServ(int fileDescriptor);
    void ReplyGrupServer(int fileDescriptor);
    bool VerifPozGrup(int &pozitie);
    void AdaugareReplyGrup(int &pozitie, string &mesaj);
};


#endif //CLION_SERVERFUNCTIONS_H