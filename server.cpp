#include "serverFunctions.h"

#define PORT 2310

extern int errno;

int main()
{
    serverFunctions srv;
    struct sockaddr_in server;
    struct sockaddr_in from;
    fd_set readfds;
    fd_set activefds;
    struct timeval timeSelect;
    int sd,client;//descriptori de socket
    int optval=1;//pentru setsockopt()
    int fileDescriptor;//descriptor pentru parcurgerea listelor de descriptori
    int nrFileDescriptors;
    socklen_t lenSock;//lungimea structurii sockaddr_in

    if( (sd=socket(AF_INET,SOCK_STREAM,0))==-1)
    {
        perror("[server] Eroare la crearea socketului!\n");
        return errno;
    }

    setsockopt(sd,SOL_SOCKET,SO_REUSEADDR,&optval,sizeof(optval));

    bzero(&server,sizeof(server));

    server.sin_family=AF_INET;
    server.sin_addr.s_addr=htonl(INADDR_ANY);
    server.sin_port=htons(PORT);

    if(bind(sd,(struct sockaddr*)&server,sizeof(struct sockaddr))==-1)
    {
        perror("[server] Eroare la bind()!\n");
        return errno;
    }

    if(listen(sd,5)==-1)
    {
        perror("[server] Eroare la listen()!\n");
        return errno;
    }


    FD_ZERO(&activefds);//am sters toate intrarile
    FD_SET(sd,&activefds);//am adaugat fileDescriptor la multimea activa de descriptori


    timeSelect.tv_sec=1;//timp de asteptare 1 secunda
    timeSelect.tv_usec=0;

    nrFileDescriptors=sd;//valoarea maxima a descriptorilor folositi

    printf("\n[server] Asteptam conectarea clientilor la portul %d\n",PORT);
    fflush(stdout);

    while(1)//servire concurenta clienti
    {
        bcopy((char *) &activefds, (char *) &readfds, sizeof(readfds));


        if (select(nrFileDescriptors + 1, &readfds, NULL, NULL, &timeSelect) < 0) {
            perror("[server] Eroare la select()!\n");
            return errno;
        }

        if (FD_ISSET(sd, &readfds))//verificam daca socketul poate accepta clienti
        {
            lenSock = sizeof(from);
            bzero(&from, sizeof(from));

            client = accept(sd, (struct sockaddr *) &from, &lenSock);//acceptam conexiunea

            if (client < 0) {
                perror("[server] Eroare la accept()!\n");
                continue;
            }

            FD_SET(client, &activefds);//adaugam socketul clientului la lista descriptorilor activi

            printf("[server] S-a conectat un client de la adresa %s\n", srv.conversie_adresa(from));
            fflush(stdout);
            int pid;
            if ((pid = fork()) == -1)
            {
                perror("[server]Eroare la fork()!\n");
                close(sd);
                continue;
            }
            else if (pid > 0)//parinte
            {

                close(client);
                FD_CLR(client, &activefds);
                while (waitpid(-1, nullptr, WNOHANG));
                continue;
            }
            else if(pid==0)//fiu
            {
                close(sd);
                fileDescriptor=client;
                char comanda[10];
                bzero(comanda, 10);
                if(read(fileDescriptor, comanda, 10)<=0)
                {
                    perror("Eroare la read in server!");
                }
                cout << "\n[server]Am primit comanda:" << comanda;
                if (strstr(comanda, "login"))
                {
                    cout << "\n[server] Clientul de la adresa " << srv.conversie_adresa(from) << " a decis sa intre in cont";
                    srv.Login(fileDescriptor);
                    srv.platforma(fileDescriptor,activefds);
                    srv.quit(fileDescriptor, activefds);
                }
                else if (strstr(comanda, "register"))
                {
                    cout << "\n[server] Clientul cu descriptorul " << srv.conversie_adresa(from) << " a decis sa isi creeze un cont nou";
                    srv.inregistrare(fileDescriptor);
                    char comanda1[10];
                    bzero(comanda1, 10);
                    read(fileDescriptor, comanda1, 10);
                    cout<<"\n[server]Am primit comanda:"<<comanda1<<endl;
                    if (strstr(comanda1, "login"))
                    {
                        srv.Login(fileDescriptor);
                        srv.platforma(fileDescriptor,activefds);
                        srv.quit(fileDescriptor, activefds);
                    }
                    else if (strstr(comanda1, "exit"))
                    {
                        srv.quit(fileDescriptor, activefds);
                    }
                }
                else if (strstr(comanda, "exit"))
                {
                    cout << "\n[server] Clientul de la adresa " << srv.conversie_adresa(from) << " a decis sa paraseasca aplicatia";
                    srv.quit(fileDescriptor, activefds);
                }
            }
        }
    }
}