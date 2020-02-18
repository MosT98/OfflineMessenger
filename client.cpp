#include "clientFunctions.h"

int port;

int main(int argc, char*argv[])
{
    int sd;
    struct sockaddr_in server;//structura pentru conectare
    char msg[100];//mesajul trimis

    if(argc != 3)
    {
        printf("[client] Sintaxa este : %s <adresa_server> <port>\n",argv[0]);
        return -1;
    }

    port=atoi(argv[2]);

    if((sd=socket(AF_INET, SOCK_STREAM,0))==-1)
    {
        perror("[client] Eroare la socket()!\n");
    }

    server.sin_family=AF_INET;//familia socketului
    server.sin_addr.s_addr=inet_addr(argv[1]);//adresa IP
    server.sin_port=htons(port);//portul de conectare

    if( connect(sd,(struct sockaddr*)&server, sizeof(struct sockaddr))==-1)
    {
        perror("[client] Eroare la connect()!\n");
    }

    while(1)
    {
        char comanda[10];
        bzero(msg,100);
        fflush(stdout);
        clientFunctions client;
        cout<<"[client]Buna ziua!\n";
        cout<<"Aveti la dispozitie urmatoarele comenzi:login/register/exit\n";

        cout<<"\nIntroduceti comanda dorita:";
        cin>>comanda;
        if(write(sd,comanda,10)<=0)
        {
            perror("Eroare la write in client!");
        }
        if(strcmp(comanda,"login")==0)
        {
            client.login(sd);
            client.platforma(sd);
            break;
        }
        else if(strcmp(comanda,"exit")==0)
        {
            client.exit();
            break;
        }
        else if(strcmp(comanda,"register")==0)
        {
            client.inregistrare(sd);
            cout<<"\nAveti la dispozitie urmatoarele comenzi:login/exit\n";
            cout<<"Introduceti comanda dorita:";
            cin>>comanda;
            write(sd,comanda,10);
            if(strcmp(comanda,"login")==0)
            {
                client.login(sd);
                client.platforma(sd);
                client.exit();
            }
            else if(strcmp(comanda,"exit")==0)
            {
                client.exit();
            }
            break;
        }
    }
    close(sd);
}