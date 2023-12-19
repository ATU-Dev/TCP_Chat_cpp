#include <iostream>
#include <WS2tcpip.h>
#include <Windows.h>
#include <string>
#include <stdio.h>
#include <vector>
#include <thread>
#include <ctime>
using namespace std;

#pragma comment(lib, "ws2_32.lib")

bool connected = false;

void nachrichtenEmpfangen(SOCKET socket, string ipAdress)
{
	char buf[4096];

	while (true)
	{
		ZeroMemory(buf, 4096);
		int bytesReceived = recv(socket, buf, 4096, 0);

		if (bytesReceived > 0)
			cout << ipAdress << ": " << string(buf, 0, bytesReceived) << endl;
	}
}

int VerbindungChecken()
{
	Sleep(2000);
	if (!connected)
	{
		system("start %cd%\\ChatClient.exe");
		exit(0);
	}
}

int main()
{
	string ipAdresse = "127.0.0.1";				// IP-Adresse vom TCP_Server
	int port = 1337;							// Port vom TCP_Server

	cout << "Bitte geben Sie die IP-Adresse vom Server: ";
	getline(cin, ipAdresse);

	// Timer für Verbindung starten
	thread VerbindenThread(VerbindungChecken);
	//VerbindungChecken(connected);

	// Winsock aktivieren
	WSAData data;
	WORD ver = MAKEWORD(2, 2);
	int wsResult = WSAStartup(ver, &data);
	if (wsResult != 0)
	{
		cerr << "Winsock konnte nicht gestartet werden, Programm wird beendet!";
		return 0;
	}

	// Socket erstellen
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		cerr << "Socket konnte nicht gestartet werden, Programm wird beendet!";
		WSACleanup();
		return 0;
	}

	// Eine hint-struct erstellen
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);
	inet_pton(AF_INET, ipAdresse.c_str(), &hint.sin_addr);

	// Server verbinden
	int connectResult = connect(sock, (sockaddr*)&hint, sizeof(hint));
	if (connectResult == SOCKET_ERROR)
	{
		cerr << "Keine Verbindung zum Server!";
		closesocket(sock);
		WSACleanup();
		return 0;
	}

	// Mit Server verbunden
	cout << "Verbindug zum Server " << ipAdresse << " hergestellt!" << endl;
	connected = true;

	// Thread für Nachrichten empfangen
	vector<thread> threads;
	threads.emplace_back(nachrichtenEmpfangen, sock, ipAdresse);

	// Schleife machen, wo Daten empfangen werden
	string eingabe;

	do {
		// Nachricht senden
		getline(cin, eingabe);
		int sendResult = send(sock, eingabe.c_str(), eingabe.size() + 1, 0);
	} while (eingabe.size() > 0);

	closesocket(sock);
	WSACleanup();
}