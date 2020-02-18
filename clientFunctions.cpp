//
// Created by most on 16.12.2018.
//

#include "clientFunctions.h"

string usernameGLOBAL;
string ID;

void clientFunctions::login(int sd)
{
    string username;
    string parola;
    cout<<"\n[client]Va rog sa introduceti username-ul:";
    cin>>username;
    cout<<"\n[client]Va rog sa introduceti parola:";
    cin>>parola;
    if((username.length()>5 && username.length() <9) && (parola.length()>5 && parola.length()<9 ))
    {
        cout<<"[client]Username-ul si parola introduse sunt valide!\n";
        char buf[2];
        bzero(buf,2);
        write(sd,username.data(),10);

        write(sd,parola.data(),10);

        read(sd,buf,2);

        if(strcmp(buf,"1")==0)
        {
            cout<<"\n\t\tLogare reusita! Bine ai venit, "<<username<<"!\n";
            usernameGLOBAL=username;
        }
        else{
            cout<<"\nDATE GRESITE\n";
            login(sd);
        }
    }
    else
    {
        cout<<"\n[client]Datele introduse nu corespunde cerintelor.\n Vom reveni la meniul de logare.\n\n";
        login(sd);
    }
}

void clientFunctions::inregistrare(int sd)
{
    char username[10], parola[10];
    bzero(username,10);
    bzero(parola,10);
    //cout<<"\n[client] Ati intrat in platforma de inregistrare\n";
    cout<<"[client] Introduceti username-ul dorit(intre 5-9 caractere):";
    cin>>username;
    cout<<"[client] Introduceti parola dorita(intre 5-9 caractere):";
    cin>>parola;
    if((strlen(username)>5 && strlen(username)<9) && (strlen(parola)>5 && strlen(parola)<9 ))
    {
        cout<<"\n[client] Datele introduse corespund cerintelor!\n";

        write(sd,username,10);
        write(sd,parola,10);

        char buf[2];
        bzero(buf,2);
        read(sd,buf,2);
        if(strcmp(buf,"1")==0)
        {
            cout<<"Ai fost adaugat in baza de date, "<<username<<endl;
        }
        else
        {
            cout<<"Username-ul introdus exista deja in baza de date!"<<endl<<"Vom reveni la meniul de inregistrare"<<endl;
            inregistrare(sd);
        }
    }
    else
    {
        cout<<"\n[client]Datele introduse nu corespunde cerintelor.\n Vom reveni la meniul de inregistrare.\n\n";
        inregistrare(sd);
    }

}

void clientFunctions::exit() {
    cout<<"\n[client] Ati decis sa parasiti aplicatia\n";
}

void clientFunctions::platforma(int sd) {
    string comanda;
    cout << "Ai la dispozitie urmatoarele comenzi:\n"
            "1. Start conversatie\n"
            "2. Start conversatie de grup\n"
            "3. Vizualizare arhiva(+reply)\n"
            "4. Vizualizare arhiva grup(+reply)\n"
            "5. Vizualizare utilizatori inregistrati in sistem\n"
            "6. Numar utilizatori inregistrati in sistem\n"
            "7. Exit\n";
    cout<<"Introduceti numarul comenzii dorite:";
    cin>>comanda;

    if(comanda.length()>2)
    {
        cout<<"Ati introdus o comanda gresita, vom reveni inapoi in meniu!";
        platforma(sd);
    }

    if(write(sd,comanda.data(),2)<=0)
    {
        perror("Eroare la write in client");
    }

    if(comanda=="1")
    {
        StartConversatie(sd);
        platforma(sd);
    }
    else if(comanda=="2")
    {
        StartConversatieGrup(sd);
        platforma(sd);
    }
    else if(comanda=="3")
    {
        AfiseazaIstoric(sd);
        platforma(sd);
    }
    else if(comanda=="4")
    {
        AfiseazaIstoricGrup(sd);
        platforma(sd);
    }
    else if(comanda=="5")
    {
        char useri[128];
        bzero(useri,128);

        if(read(sd,useri,128)<=0)
        {
            perror("Eroare la read in client");
        }

        cout<<"\nIn acest moment, in baza de date exista urmatorii utilizatori: "<<useri<<endl;
        platforma(sd);

    }
    else if(comanda=="6")
    {
        char numar[3];
        bzero(numar,3);
        if(read(sd,numar,3)<=0)
        {
            perror("Eroare la read in client!");
        }
        cout<<"\nIn acest moment, in baza de date exista "<<numar<<" utilizatori inregistrati.\n";
        platforma(sd);
    }
    else if(comanda=="7")
    {
        exit();
    }

}

void clientFunctions::String_Write(const string &input, int sd)
{
    unsigned long dimensiune=input.size();
    string charLen;
    charLen=to_string(dimensiune);

    write(sd,charLen.data(),10);

    write(sd,input.data(),dimensiune);

}

void clientFunctions::String_Read(string &output, int sd)
{
    string inputSize;

    read(sd,inputSize.data(),10);

    auto lungime=atoi(inputSize.data());

    char input[10000];
    bzero(input,10000);
    read(sd,input,lungime);

    output=input;

}

int MySelect(bool& refreshed) {
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(0, &readfds);
    timeval time;
    time.tv_sec = 5;
    time.tv_usec = 0;
    int r = select(1, &readfds, nullptr, nullptr, &time);
    if (r < 0) {
        return r;
    }
    refreshed = time.tv_usec == 0 && time.tv_sec == 0;

    return 1;
}

void clientFunctions::SchimbReplici(int sd)
{
    string myUsername=usernameGLOBAL;
    string mesajDeTrimis;
    cout<<"Conversatia se va opri la introducerea *stop* \n";
    cin.ignore();
    do{
        bool timeout;
        MySelect(timeout);
        if (timeout) {
            // trimiti refresh
            String_Write("refresh", sd);
            string out;
            String_Read(out,sd);
            cout<<out;
            continue;
        }
        getline(cin,mesajDeTrimis);
        String_Write(mesajDeTrimis,sd);
    }while(mesajDeTrimis!="stop");
}

void clientFunctions::IstoricMesaje(int sd, string &username)
{
    string istoric;
    String_Read(istoric,sd);
    cout<<istoric;
}

void clientFunctions::ConversatieNouaClient(int sd)
{
    string myUsername;
    String_Read(myUsername,sd);
    SchimbReplici(sd);

}

bool clientFunctions::StartConversatie(int sd)
{
    string nume;
    cout<<"Introdu numele utilizatorului cu care vrei sa incepi conversatia:\n";
    cout<<"Username:";
    cin>>nume;
    String_Write(nume,sd);

    string verificare;
    String_Read(verificare,sd);
    if(verificare=="fail")
    {
        cout<<"Username-ul introdus este gresit\n";
        return false;
    }

    ConversatieNouaClient(sd);

    return true;
}

bool clientFunctions::AfiseazaIstoric(int sd)
{
    string numeD;
    cout<<"Introdu numele utilizatorului pentru a vedea istoricul mesajelor:\n";
    cout<<"Username:";
    cin>>numeD;
    String_Write(numeD,sd);

    string verif;
    String_Read(verif,sd);
    if(verif=="fail")
    {
        cout<<"Username-ul introdus este gresit\n";
        AfiseazaIstoric(sd);
        return false;
    }

        IstoricMesaje(sd,numeD);
        Reply(sd);

    return true;
}

bool clientFunctions::Reply(int sd)
{
    string pozitieR,mesaj;
    cout<<"\nIntroduceti numarul mesajului la care vreti sa dati reply:";
    cin>>pozitieR;

    String_Write(pozitieR,sd);

    string verifPoz;

    String_Read(verifPoz,sd);

    if(verifPoz=="succes")
    {
        cout<<"\nIntroduceti mesajul care vreti sa fie adaugat:";
        cin.ignore();
        getline(cin,mesaj);

        String_Write(mesaj,sd);

    }
    else
    {
        cout<<"FOARTE GRESIT";
        Reply(sd);
    }

    return true;
}

bool clientFunctions::StartConversatieGrup(int sd)
{
    string mesajCuUsername;
    cout<<"\nIntroduceti doar utilizatori care au un cont existent in platforma!!!";
    cout<<"\nPentru a termina, introduceti *stop*.\n";
    cout<<"\nIntrodu numele utilizatorilor cu care vrei sa incepi conversatia de grup:";
    do{
        getline(cin,mesajCuUsername);
        String_Write(mesajCuUsername,sd);
    }while(mesajCuUsername!="stop");

    SchimbRepliciGrup(sd);

    return true;
}

void clientFunctions::SchimbRepliciGrup(int sd)
{
    cout<<"\nConversatia se va opri la introducerea *stop*\n";
    string msjDeTrimis;

    do{
        bool timeout;
        MySelect(timeout); // TODO: verifica eroare
        if (timeout) {
            // trimiti refresh
            String_Write("refresh", sd);
            string out;
            String_Read(out,sd);
            cout<<out;
            continue;
        }
        getline(cin,msjDeTrimis);
        String_Write(msjDeTrimis,sd);
    }while(msjDeTrimis!="stop");
}

void clientFunctions::AfiseazaIstoricGrup(int sd)
{
    string numeDest;
    cout<<"\nIntroduceti doar utilizatori care au un cont existent in platforma!!!";
    cout<<"\nPentru a termina, introduceti *stop*.\n";
    cout<<"Introdu numele utilizatorului pentru a vedea istoricul mesajelor:\n";
    do{
        getline(cin,numeDest);
        String_Write(numeDest,sd);
    }while(numeDest!="stop");

    string ist;
    String_Read(ist,sd);
    cout<<ist;

    ReplyGrup(sd);


}

void clientFunctions::ReplyGrup(int sd)
{
    string pozitieR,mesaj;
    cout<<"\nIntroduceti numarul mesajului la care vreti sa dati reply:";
    cin>>pozitieR;

    String_Write(pozitieR,sd);

    string verifPoz;

    String_Read(verifPoz,sd);

    if(verifPoz=="succes")
    {
        cout<<"\nIntroduceti mesajul care vreti sa fie adaugat:";
        cin.ignore();
        getline(cin,mesaj);

        String_Write(mesaj,sd);
    }
    else
    {
        cout<<"Ati introdus o pozitie gresita!";
        ReplyGrup(sd);
    }

}