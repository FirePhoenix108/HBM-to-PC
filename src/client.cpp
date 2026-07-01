#define _WIN32_WINNT 0x0501
#define _WINSOCK_DEPRECATED_NO_WARNING
#include <WinSock2.h>
#include <iostream>
#include <string>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

int main() {

	string ipScelto;
	string messaggioDaInviare;
	int portaScelta;

	WSAData wsaData = { 0 };
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		cout<<"Errore di inizializzazione scoket. " << WSAGetLastError() << "" <<endl;
		return 1;
	}

	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (clientSocket == INVALID_SOCKET) {
			cout<<"Errore nella creazione del socket. "<< WSAGetLastError() << "" <<endl;
			WSACleanup();
			return 1;
		}

		cout<<"Inserisci l'IP: "<<endl;
		cin>>ipScelto;
		cin.ignore(1024, '\n');

		cout<<"Inserisci la porta: "<<endl;
		cin>>portaScelta;
		cin.ignore(1024, '\n');

		sockaddr_in clientAddr;
		clientAddr.sin_family = AF_INET;
		clientAddr.sin_port = htons(portaScelta);
		clientAddr.sin_addr.s_addr = inet_addr(ipScelto.c_str());

		if(connect(clientSocket, (sockaddr*)&clientAddr, sizeof(clientAddr)) == SOCKET_ERROR){
			cout<< "Impossibile connettersi. " << WSAGetLastError() << "" <<endl;
			WSACleanup();
			cin.get();
			cin.ignore(1024, '\n');
			return 1;
		}

		cout<<"Connesso con successo."<<endl;

		cout<<"Scrivi il messaggio: "<<endl;
		while(messaggioDaInviare != "EXIT"){
			getline(cin, messaggioDaInviare);

			int byteInviati = send(clientSocket, messaggioDaInviare.c_str(), messaggioDaInviare.length(), 0);

			if(byteInviati == SOCKET_ERROR){
				cout<<"Errore durante l'invio del messaggio. " << WSAGetLastError() << "" <<endl;
			}
		}
		

		closesocket(clientSocket);
		WSACleanup();

		cout<<"Chiusura programma...";
		cin.get();

	return 0;
}