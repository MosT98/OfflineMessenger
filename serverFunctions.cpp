//
// Created by most on 16.12.2018.
//

#include "serverFunctions.h"
using json=nlohmann::json;

    string usernameGLOBALsrv;
    vector<string> usernamesGrup;
    vector<string> usernamesReply;
    string IDsrv;

char* serverFunctions::conversie_adresa(struct sockaddr_in adresa)
{
    static char str[25];
    char port[7];

    strcpy(str,inet_ntoa(adresa.sin_addr));
    bzero(port,7);

    sprintf(port, ":%d", ntohs(adresa.sin_port));

    strcat(str,port);

    return (str);
}


void serverFunctions::Login(int fileDescriptor)
{
    char username[10],parola[10];
    bzero(username,10);
    bzero(parola,10);

    read(fileDescriptor,username,10);

    cout<<"\n[server]Am primit username:"<<username;

    read(fileDescriptor,parola,10);

    cout<<"\n[server]Am primit parola:"<<parola;

    string ver=verificareUsername(username);

    if(strstr(ver.data(),parola))
    {
        cout<<"\nFELICITARI\n";
        write(fileDescriptor,"1",2);

        usernameGLOBALsrv=username;
    }
    else
    {
        cout<<"\n[server] Datele sunt gresite\n";

        write(fileDescriptor,"0",2);
        Login(fileDescriptor);

    }
}

void serverFunctions::quit(int fileDescriptor,fd_set &activefds)
{
    cout<<"\n[server] S-a deconectat un client"<<".\n";
    fflush(stdout);
    close(fileDescriptor);
    FD_CLR(fileDescriptor,&activefds);//elimin descriptorul din lista celor activi
    exit(0);
}

string serverFunctions::verificareUsername(string usr) {

    ifstream users("../users.json");
    json j=json::parse(users);
    for(auto it : j)
    {

        string username=it["username"];
        string pass=it["parola"];
        if(usr==username)
        {
            users.close();
            return pass;//returnam parola direct daca avem match pe nume
        }
    }
    users.close();
    return "fail";
}

bool serverFunctions::verifJson(string username)
{
    ifstream users("../users.json");
    json j=json::parse(users);
    for(auto it:j)
    {
        string usr=it["username"];
        if(username==usr)
        {
            users.close();
            return 0;//daca gasim un client cu acelasi username in baza de date
        }
    }

    users.close();
    return 1;//altfel
}

void serverFunctions::inregistrare(int fileDescriptor)
{
    char username[10],parola[10];
    bzero(username,10);
    bzero(parola,10);

    read(fileDescriptor,username,10);
    read(fileDescriptor,parola,10);

    cout<<"\n[server]Am primit username:"<<username;
    cout<<"\n[server]Am primit parola:"<<parola;

    if(verifJson(username)==0)
    {
        cout<<"\n[server]Exista un client cu acelasi nume in baza de date\n";
        write(fileDescriptor,"0",2);

        inregistrare(fileDescriptor);
    }
    else
    {
        write(fileDescriptor,"1",2);
        ifstream users("../users.json");
        json j=json::parse(users);

        unsigned int index=j.size();
        index++;
        j[to_string(index)]["id"]=to_string(index*10);
        j[to_string(index)]["username"]=username;
        j[to_string(index)]["parola"]=parola;
        ofstream users_out("../users.json");
        users_out<<j.dump(2);
        users.close();
        users_out.close();
        cout<<endl<<"AM ACTUALIZAT"<<endl<<endl;
    }

}



void serverFunctions::platforma(int fileDescriptor,fd_set &activefds)
{
    char buffer[2];
    bzero(buffer,2);

    read(fileDescriptor,buffer,2);
    cout<<"\n[server]Am primit comanda:"<<buffer<<endl;

    if(strstr(buffer,"1"))
    {
        StartConversatieServer(fileDescriptor);
        platforma(fileDescriptor,activefds);

    }
    else if(strstr(buffer,"2"))
    {
        StartConversatieGrupServer(fileDescriptor);
        platforma(fileDescriptor,activefds);
    }
    else if(strstr(buffer,"3"))
    {
        AfiseazaIstoricServer(fileDescriptor);
        platforma(fileDescriptor,activefds);
    }
    else if(strstr(buffer,"4"))
    {
        AfiseazaIstoricGrupServ(fileDescriptor);
        platforma(fileDescriptor,activefds);
    }
    else if(strstr(buffer,"5"))
    {
        string users;
        users=printUtilizatoriInregistrati();

        write(fileDescriptor,users.data(),128);
        platforma(fileDescriptor,activefds);
    }
    else if(strstr(buffer,"6"))
    {
        string nr;
        nr=utilizatoriInregistrati();
        write(fileDescriptor,nr.data(),3);
        platforma(fileDescriptor,activefds);
    }
    else if(strstr(buffer,"7"))
    {
        quit(fileDescriptor,activefds);
    }

}

string serverFunctions::utilizatoriInregistrati()
{
    ifstream users("../users.json");
    json j=json::parse(users);

    return (to_string(j.size()));
}

string serverFunctions::printUtilizatoriInregistrati()
{
    ifstream utilizatori("../users.json");
    json it=json::parse(utilizatori);
    vector<string> usernames;
    string usr;
    for(auto i : it)
    {
        usernames.push_back(i["username"]);
    }
    utilizatori.close();
    for(int i=0;i<usernames.size();i++)
    {
        usr=usr+usernames[i]+" ";
    }
    return usr;
}

void serverFunctions::Format_Read(string &output, int fileDescriptor)
{
    string inputSize;

    read(fileDescriptor,inputSize.data(),10);

    auto lungime=atoi(inputSize.data());

    char input[10000];
    bzero(input,10000);
    read(fileDescriptor,input,lungime);

    output=input;

}

void serverFunctions::Format_Write(string &input, int fileDescriptor)
{
    unsigned long dimensiune=input.size();
    string charLen;
    charLen=to_string(dimensiune);
    write(fileDescriptor,charLen.data(),10);

    write(fileDescriptor,input.data(),dimensiune);

}

string serverFunctions::getIDusr(string &username) {
    ifstream users("../users.json");
    json j=json::parse(users);

    for(auto it : j)
    {

        string usr=it["username"];
        string ID=it["id"];
        if(usr==username)
        {
            users.close();
            return ID;
        }
    }
    users.close();
    return "fail";
}

bool serverFunctions::VerifConversatie(int fileDescriptor,string &destinatar)
{
    string user1=usernameGLOBALsrv;
    string user2=destinatar;

    ifstream conversatie("../conversatii.json");
    json j=json::parse(conversatie);
    conversatie.close();

    for(auto &it : j["Conversatii"])
    {
        if( ( it["user1"]==user1 && it["user2"]==user2 ) ||( it["user1"]==user2 && it["user2"]==user1) )
        {
            return true;
        }
    }

    return false;
}

void serverFunctions::UpdateConversatie(string &mesaj, string &destinatar)
{
    time_t rawtime;
    struct tm * timeinfo;
    time (&rawtime);
    timeinfo = localtime (&rawtime);
    string user1=usernameGLOBALsrv;
    string user2=destinatar;

    char data[30];
    sprintf(data,"%s",asctime(timeinfo));

    string mesajdata(data);
    string mesajNOU;
    mesajNOU.assign(mesajdata, 0, mesajdata.size()-6);

    mesajNOU+="<"+user1+"> ";
    mesajNOU+=mesaj;

    ifstream conv("../conversatii.json");
    json j=json::parse(conv);
    conv.close();

    for(auto &it : j["Conversatii"])
    {
        if( ( it["user1"]==user1 && it["user2"]==user2 ) ||( it["user1"]==user2 && it["user2"]==user1) )
        {
            string numarMesaje=it["nrMesaje"];
            int numarMesajeINT=atoi(numarMesaje.data());
            numarMesajeINT++;
            string numarMesajeActualizat=to_string(numarMesajeINT);

            vector<string> VectMesaje=it["Replici"];
            VectMesaje.push_back(mesajNOU);
            it["nrMesaje"]=numarMesajeActualizat;
            it["Replici"]=VectMesaje;

            ofstream conv_out("../conversatii.json");
            conv_out<<j.dump(2);
            conv_out.close();
            break;
        }
    }
}

void serverFunctions::SchimbRepliciServer(int fileDescriptor, string &destinatar)
{
    string mesajPrimit;
    int nrmsg=0;
    Format_Read(mesajPrimit,fileDescriptor);

    while(mesajPrimit!="stop")
    {
        if(mesajPrimit=="refresh")
        {
            string refresh=RefreshConversatie(destinatar,nrmsg);
            Format_Write(refresh,fileDescriptor);
        }
        else
        {
            if(VerifConversatie(fileDescriptor, destinatar))
            {
                UpdateConversatie(mesajPrimit,destinatar);
            }
            else
            {
                CreareConversatie(destinatar);
                UpdateConversatie(mesajPrimit,destinatar);
            }
        }

        Format_Read(mesajPrimit,fileDescriptor);
    }

}

string serverFunctions::RefreshConversatie(string &destinatar, int &nrmsg)
{
    string MesajOutput;
    ifstream conversatie("../conversatii.json");
    json j=json::parse(conversatie);
    conversatie.close();

        for(auto it : j[ "Conversatii"])
        {
        if( (it["user1"]==usernameGLOBALsrv && it["user2"]==destinatar ) || (it["user1"]==destinatar && it["user2"]==usernameGLOBALsrv) )
        {
            vector<string> mesaje = it["Replici"];
            for (int index = 0; index < mesaje.size(); index++)
            {
                string MesajCurent = mesaje[index];
                string scotUSR;
                scotUSR.assign(MesajCurent,20,usernameGLOBALsrv.size());
                if(index>=nrmsg)
                {
                    if(scotUSR!=usernameGLOBALsrv)
                    {
                        MesajOutput += MesajCurent;
                        MesajOutput += "\n";
                        nrmsg++;
                    }
                    else
                        nrmsg++;

                }
            }
        }
        }

    return MesajOutput;
}

void serverFunctions::IstoricMesajeServer(int fileDescriptor, string &username)
{
    string dest=username;
    string Convorbire=IstoricConversatie(dest);
    Format_Write(Convorbire,fileDescriptor);
}

void serverFunctions::ConversatieNouaServer(int fileDescriptor, string &destinatar)
{
    string myUsername=usernameGLOBALsrv;
    Format_Write(myUsername,fileDescriptor);
    SchimbRepliciServer(fileDescriptor,destinatar);

}

bool serverFunctions::StartConversatieServer(int fileDescriptor)
{
    string name;
    Format_Read(name,fileDescriptor);
    string ID=getIDusr(name);
    if(ID=="fail")
        return false;

    Format_Write(ID,fileDescriptor);

    ConversatieNouaServer(fileDescriptor,name);

    return true;

}

void serverFunctions::CreareConversatie(string &destinatar)
{
    ifstream conv("../conversatii.json");
    json j=json::parse(conv);
    conv.close();

    string numar=to_string(j["Conversatii"].size()+1);

    j["Conversatii"][numar]["user1"]=usernameGLOBALsrv;
    j["Conversatii"][numar]["user2"]=destinatar;
    j["Conversatii"][numar]["NrConversatie"]=numar;
    vector<string> VctNull;
    j["Conversatii"][numar]["Replici"]=VctNull;
    j["Conversatii"][numar]["nrMesaje"]="0";

    ofstream conv_out("../conversatii.json");
    conv_out<<j.dump(2);
    conv_out.close();

}

string serverFunctions::IstoricConversatie(string &destinatar)
{
    string MesajOutput;

    ifstream conversatie("../conversatii.json");
    json j=json::parse(conversatie);
    conversatie.close();

    for(auto it : j["Conversatii"])
    {
        if( (it["user1"]==usernameGLOBALsrv && it["user2"]==destinatar ) || (it["user1"]==destinatar && it["user2"]==usernameGLOBALsrv) )
        {
            vector<string> mesaje = it["Replici"];
            for (int i = 0; i < mesaje.size(); i++)
            {
                string MesajCurent = mesaje[i];
                MesajOutput +="~"+to_string(i)+"~ "+MesajCurent;
                MesajOutput += "\n";
            }
        }
    }

    return MesajOutput;

}

bool serverFunctions::AfiseazaIstoricServer(int fileDescriptor)
{
    string nameDest;
    Format_Read(nameDest,fileDescriptor);

    string ID=getIDusr(nameDest);

    Format_Write(ID,fileDescriptor);

    if(ID=="fail")
    {
        AfiseazaIstoricServer(fileDescriptor);
    }

    IstoricMesajeServer(fileDescriptor,nameDest);

    ReplyServer(fileDescriptor,nameDest);


    return true;
}

void serverFunctions::ReplyMesajServer(int &pozitie, string &mesaj, string &destinatar)
{
    time_t rawtime;
    struct tm * timeinfo;
    time (&rawtime);
    timeinfo = localtime (&rawtime);
    string mesajDeAdaugat;
    char data[30];
    sprintf(data,"%s",asctime(timeinfo));

    ifstream conversatie("../conversatii.json");
    json j=json::parse(conversatie);
    conversatie.close();

    mesajDeAdaugat+="       ";

    string mesajDATA;
    string mesajdata(data);
    mesajDATA.assign(mesajdata, 0, mesajdata.size()-6);

    mesajDeAdaugat+=mesajDATA;
    mesajDeAdaugat+="<"+usernameGLOBALsrv+">";
    mesajDeAdaugat+=" ";
    mesajDeAdaugat+=mesaj;

    for(auto &it : j["Conversatii"])
    {
        if( (it["user1"]==usernameGLOBALsrv && it["user2"]==destinatar ) || (it["user1"]==destinatar && it["user2"]==usernameGLOBALsrv) )
        {
            vector<string> msgs=it["Replici"];

                string numarMesaje=it["nrMesaje"];
                int numarMesajeINT=atoi(numarMesaje.data());
                numarMesajeINT++;
                string numarMesajeActualizat=to_string(numarMesajeINT);

                msgs.insert(msgs.begin()+pozitie+1,mesajDeAdaugat);

                it["nrMesaje"]=numarMesajeActualizat;
                it["Replici"]=msgs;

                ofstream conversatie_out("../conversatii.json");
                conversatie_out<<j.dump(2);
                conversatie_out.close();

            break;
        }
    }

}

bool serverFunctions::ReplyServer(int fileDescriptor, string &destinatar)
{
    string poz,mesajReply;
    int pozitie;


    Format_Read(poz,fileDescriptor);

    pozitie = atoi(poz.c_str());

    if(VerifPoz(destinatar,pozitie))
    {
        string abc="succes";
        Format_Write(abc,fileDescriptor);

        Format_Read(mesajReply,fileDescriptor);

        ReplyMesajServer(pozitie,mesajReply,destinatar);
    }
    else
    {
        string bca="fail";
        Format_Write(bca,fileDescriptor);
        ReplyServer(fileDescriptor,destinatar);
    }

    return true;
}

bool serverFunctions::VerifPoz(string &destinatar, int &pozitie)
{
    ifstream conversatie("../conversatii.json");
    json j=json::parse(conversatie);
    conversatie.close();

    for(auto it : j["Conversatii"])
    {
        if( (it["user1"]==usernameGLOBALsrv && it["user2"]==destinatar ) || (it["user1"]==destinatar && it["user2"]==usernameGLOBALsrv) )
        {
            string poz=it["nrMesaje"];
            int nrPoz=atoi(poz.c_str());
            if(pozitie>nrPoz)
                return false;
        }
    }
    return true;
}

bool serverFunctions::StartConversatieGrupServer(int fileDescriptor)
{
    string useriConv;
    usernamesGrup.push_back(usernameGLOBALsrv);
    Format_Read(useriConv,fileDescriptor);
    while(useriConv!="stop")
    {
        if(getIDusr(useriConv)!="fail")
        {
            usernamesGrup.push_back(useriConv);
        }
        Format_Read(useriConv,fileDescriptor);
    }

    SchimbRepliciGrupServer(fileDescriptor);


    return true;
}

void serverFunctions::SchimbRepliciGrupServer(int fileDescriptor)
{

    string mesajPrimit;
    int nrmsg=0;
    Format_Read(mesajPrimit,fileDescriptor);

    while(mesajPrimit!="stop")
    {
        if(mesajPrimit=="refresh")
        {
            string refresh=RefreshConvGrup(nrmsg);
            Format_Write(refresh,fileDescriptor);
        }
        else
        {
            if(VerifConvGrup())//TODO:daca au aceleasi nume ca in json, indiferent de ordine
            {
                UpdateConvGrup(mesajPrimit);//TODO:de facut functia
            }
            else
            {
                CreareConvGrup();
                UpdateConvGrup(mesajPrimit);//TODO:de facut functia
            }
        }

        Format_Read(mesajPrimit,fileDescriptor);
    }



}

bool VerifVectori(vector<string>& usernames, vector<string>& usernamesgr)
{
    if(usernames.size()!=usernamesgr.size())
        return false;
    for (auto us:usernames)
    {
        bool check=false;
        for(auto usg:usernamesgr)
            if(us==usg)
            {
                check=true;
                break;
            }
        if(check==false)
            return false;
    }
    return true;
}

bool serverFunctions::VerifConvGrup()
{
    ifstream conversatie("../conversatiiGrup.json");
    json j=json::parse(conversatie);
    conversatie.close();

    for(auto &it:j["ConversatiiGrup"])
    {
        vector<string> usernames=it["Useri"];
        if(VerifVectori(usernames, usernamesGrup))
            return true;
    }
    return false;
}

void serverFunctions::CreareConvGrup()
{
    ifstream conversatie("../conversatiiGrup.json");
    json j=json::parse(conversatie);
    conversatie.close();

    string numar=to_string(j["ConversatiiGrup"].size()+1);

    vector<string> VectorUseri;
    //VectorUseri.push_back(usernameGLOBALsrv);
    for(unsigned int i=0;i<usernamesGrup.size();i++)
        VectorUseri.push_back(usernamesGrup[i]);
    j["ConversatiiGrup"][numar]["Useri"]=VectorUseri;
    j["ConversatiiGrup"][numar]["NrConversatie"]=numar;
    vector<string> VctNull;
    j["ConversatiiGrup"][numar]["Replici"]=VctNull;
    j["ConversatiiGrup"][numar]["nrMesaje"]="0";

    ofstream conv_out("../conversatiiGrup.json");
    conv_out<<j.dump(2);
    conv_out.close();

}

void serverFunctions::UpdateConvGrup(string &mesaj)
{
    time_t rawtime;
    struct tm * timeinfo;
    time (&rawtime);
    timeinfo = localtime (&rawtime);

    char data[30];
    sprintf(data,"%s",asctime(timeinfo));

    string mesajdata(data);
    string mesajNOU;
    mesajNOU.assign(mesajdata, 0, mesajdata.size()-6);

    mesajNOU+="<"+usernameGLOBALsrv+"> ";
    mesajNOU+=mesaj;

    ifstream conv("../conversatiiGrup.json");
    json j=json::parse(conv);
    conv.close();

    for(auto &it : j["ConversatiiGrup"])
    {
        vector<string> usernames=it["Useri"];
        if(VerifVectori(usernames, usernamesGrup))
        {
            string numarMesaje=it["nrMesaje"];
            int numarMesajeINT=atoi(numarMesaje.data());
            numarMesajeINT++;
            string numarMesajeActualizat=to_string(numarMesajeINT);

            vector<string> VectMesaje=it["Replici"];
            VectMesaje.push_back(mesajNOU);
            it["nrMesaje"]=numarMesajeActualizat;
            it["Replici"]=VectMesaje;

            ofstream conv_out("../conversatiiGrup.json");
            conv_out<<j.dump(2);
            conv_out.close();
            break;
        }
    }

}

string serverFunctions::RefreshConvGrup(int &nrmsg)
{
    string MesajOutput;
    ifstream conversatie("../conversatiiGrup.json");
    json j=json::parse(conversatie);
    conversatie.close();

    for(auto it : j[ "ConversatiiGrup"])
    {
        vector<string> usernames=it["Useri"];
        if(VerifVectori(usernames, usernamesGrup))
        {
            vector<string> mesaje = it["Replici"];
            for (int index = 0; index < mesaje.size(); index++)
            {
                string MesajCurent = mesaje[index];
                string scotUSR;
                scotUSR.assign(MesajCurent,20,usernameGLOBALsrv.size());
                if(index>=nrmsg)
                {
                    if(scotUSR!=usernameGLOBALsrv)
                    {
                        MesajOutput += MesajCurent;
                        MesajOutput += "\n";
                        nrmsg++;
                    }
                    else
                        nrmsg++;

                }
            }
        }
    }

    return MesajOutput;
}

void serverFunctions::AfiseazaIstoricGrupServ(int fileDescriptor)
{
    string useriReply;
    usernamesReply.push_back(usernameGLOBALsrv);
    Format_Read(useriReply,fileDescriptor);
    while(useriReply!="stop")
    {
        if(getIDusr(useriReply)!="fail")
        {
            usernamesReply.push_back(useriReply);
        }
        Format_Read(useriReply,fileDescriptor);
    }

    string istoric=IstoricMesajeGrupServ(fileDescriptor);

    Format_Write(istoric,fileDescriptor);

    ReplyGrupServer(fileDescriptor);

}

string serverFunctions::IstoricMesajeGrupServ(int fileDescriptor)
{
    string MesajOutput;

    ifstream conversatie("../conversatiiGrup.json");
    json j=json::parse(conversatie);
    conversatie.close();

    for(auto &it : j["ConversatiiGrup"])
    {
        vector<string> usernames=it["Useri"];
        if(VerifVectori(usernames, usernamesReply))
        {
            vector<string> mesaje = it["Replici"];
            for (int i = 0; i < mesaje.size(); i++)
            {
                string MesajCurent = mesaje[i];
                MesajOutput +="~"+to_string(i)+"~ "+MesajCurent;
                MesajOutput += "\n";
            }
        }
    }

    return MesajOutput;

}

void serverFunctions::ReplyGrupServer(int fileDescriptor)
{
    string poz,mesajReply;
    int pozitie;

    Format_Read(poz,fileDescriptor);

    pozitie = atoi(poz.c_str());
    cout<<"poz:"<<pozitie<<endl<<endl;
    if(VerifPozGrup(pozitie))
    {
        string abc="succes";
        Format_Write(abc,fileDescriptor);

        Format_Read(mesajReply,fileDescriptor);
        AdaugareReplyGrup(pozitie,mesajReply);
    }
    else
    {
        string bca="fail";
        Format_Write(bca,fileDescriptor);
        ReplyGrupServer(fileDescriptor);
    }

}

bool serverFunctions::VerifPozGrup(int &pozitie)
{
    ifstream conversatie("../conversatiiGrup.json");
    json j=json::parse(conversatie);
    conversatie.close();

    for(auto it : j["ConversatiiGrup"])
    {
        vector<string> usernames=it["Useri"];
        if(VerifVectori(usernames, usernamesReply))
        {
            string poz=it["nrMesaje"];
            int nrPoz=atoi(poz.c_str());
            if(pozitie>nrPoz)
                return false;
        }
    }
    return true;
}

void serverFunctions::AdaugareReplyGrup(int &pozitie, string &mesaj)
{
    time_t rawtime;
    struct tm * timeinfo;
    time (&rawtime);
    timeinfo = localtime (&rawtime);
    string mesajDeAdaugat;
    char data[30];
    sprintf(data,"%s",asctime(timeinfo));

    ifstream conversatie("../conversatiiGrup.json");
    json j=json::parse(conversatie);
    conversatie.close();

    mesajDeAdaugat+="       ";

    string mesajDATA;
    string mesajdata(data);
    mesajDATA.assign(mesajdata, 0, mesajdata.size()-6);

    mesajDeAdaugat+=mesajDATA;
    mesajDeAdaugat+="<"+usernameGLOBALsrv+">";
    mesajDeAdaugat+=" ";
    mesajDeAdaugat+=mesaj;

    for(auto &it : j["ConversatiiGrup"])
    {
        vector<string> usernames=it["Useri"];
        if(VerifVectori(usernames, usernamesReply))
        {
            vector<string> msgs=it["Replici"];

            string numarMesaje=it["nrMesaje"];
            int numarMesajeINT=atoi(numarMesaje.data());
            numarMesajeINT++;
            string numarMesajeActualizat=to_string(numarMesajeINT);

            msgs.insert(msgs.begin()+pozitie+1,mesajDeAdaugat);

            it["nrMesaje"]=numarMesajeActualizat;
            it["Replici"]=msgs;

            ofstream conversatie_out("../conversatiiGrup.json");
            conversatie_out<<j.dump(2);
            conversatie_out.close();

            break;
        }
    }

}
