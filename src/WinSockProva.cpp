#define _WIN32_WINNT 0x0501
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <windows.h>
#include <iostream>
#include <string>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

int main() {

    string ipScelto;
    int portaScelta;
    bool inFunzione = true;

    //INIZIALIZZAZIONE DI WINSOCK
    WSADATA wsaData = { 0 };
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cout << "Errore di inizzializzazione winsock!" << endl;
        return 1;
    }

    //CREAZIONE DEL SOCKET DEL SERVER
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        cout << "Errore nella creazione del socket!" << endl;
        WSACleanup();
        return 1;
    }

    cout << "Inserisci l'indirizzo IP che si vuole usare (IPv4): " << endl;
    cin >> ipScelto;

    cout << "Inserisci la porta che si vuole usare (1 a 65535): " << endl;
    cin >> portaScelta;

    //CONFIGURAZIONE INDIRIZZO IP E PORTA
    sockaddr_in serverAddr; //Struct sockaddr_in con nome dato serverAddr
    serverAddr.sin_family = AF_INET; //Dice che tipo di IP vogliamo usare. In questo caso IPv4 (AF_INET. Se era AF_INET6 era IPv6)
    serverAddr.sin_port = htons(portaScelta); //Specifica quale porta vogliamo usare tra una scelta da 1 a 65535
    serverAddr.sin_addr.s_addr = inet_addr(ipScelto.c_str()); //Specifica quale indirizzo IP usare. In questo caso con INADDR_ANY accetta tutti gli IP.


    //BIND (Lega il socket all'indirizzo appena configurato)
    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cout<<"Errore nel bind. "<< WSAGetLastError() << "" << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    //LISTEN
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        cout<<"Errore nel listen."<<endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }


    //ACCEPT (Blocca il programma finchè un client non si connette)
    cout<<"\n1/2 Connessione al client..."<<endl;
    SOCKET clientSocket = accept(serverSocket, NULL, NULL);
    if (clientSocket == INVALID_SOCKET) {
        cout << "Errore nell'accettare la connessione con il client." << WSAGetLastError() << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    cout<<"\n2/2 Connessione al client LUA"<<endl;
    SOCKET clientMinecraft = accept(serverSocket, NULL, NULL);
    if(clientSocket == INVALID_SOCKET){
        cout << "Errore nell'accettare la connessione dal client LUA. " << WSAGetLastError() << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    cout << "\nI client sono connessi con successo." << endl;
    

    while(inFunzione){

        //RICEZIONE DATI
        char buffer[1024] = { 0 };//Contenitore per il messaggio del client
        int byteRicevuti = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);


        if (byteRicevuti > 0) {
            cout << "Messaggio ricevuto dal client: " << buffer << endl;
            string controlloMess(buffer);
            controlloMess += "\n";

            //Manda il messaggio al client di LUA
            send(clientMinecraft, controlloMess.c_str(), controlloMess.length(), 0);
            if(controlloMess == "EXIT"){
                cout<<"Comando EXIT rilevato dal client. Il server si chiudera' a breve..."<<endl;
                inFunzione = false;
                Sleep(200);

            }
        } else if (byteRicevuti == 0) {
            cout<< "Client disconnesso."<<endl;
        } else {
            cout<<"Errore nella ricezione dati. "<< WSAGetLastError() << "" <<endl;
        }

        Sleep(100);
    }
        

    //CHIUSURA E PULIZIA
    closesocket(clientSocket);//Chiude la chat con il client
    closesocket(serverSocket);//Chiude il server
    WSACleanup(); //Spegne la libreria di rete


    cout << "Programma terminato." << endl;
    cin.ignore(1024, '\n');
    cin.get();

    return 0;
}