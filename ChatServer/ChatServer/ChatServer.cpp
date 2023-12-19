#include <iostream>
#include <WS2tcpip.h>
#include <winsock2.h>
#include <string>
#include <thread>
#include <vector>

using namespace std;

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable : 4996)

SOCKET* clientSocket = new SOCKET[16];

void clientsThread(SOCKET clientSocket, int clientIndex)
{
	char buf[4096];

	while (true)
	{
		ZeroMemory(buf, 4096);
		int bytesReceived = recv(clientSocket, buf, 4096, 0);

		if (bytesReceived == SOCKET_ERROR)
		{
			cerr << "Fehler in recv()!" << endl;
			break;
		}
		if (bytesReceived == 0)
		{
			cerr << "Der Client wurde getrennt!" << endl;
			break;
		}

		// Nachricht am Konsole ausgeben
		cout << "Client " << clientIndex << ": " << string(buf, 0, bytesReceived) << endl;
	}
}

void getClients()
{
	// Winsock aktivieren
	WSADATA wsD;
	WORD ver = MAKEWORD(2, 2);

	int wsOk = WSAStartup(ver, &wsD);
	if (wsOk != 0)
	{
		cerr << "Winsock konnte nicht gestartet werden !";
		return;
	}

	// SOCKET oder Listener erstellen
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET)
	{
		cerr << "Listener konnte nicht gestartet werden !";
		return;
	}

	// SOCKET an IP und PORT binden
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(1337);
	hint.sin_addr.S_un.S_addr = INADDR_ANY; // Jede(lokal & extern) IP-Adresse

	bind(listening, (sockaddr*)&hint, sizeof(hint));
	listen(listening, SOMAXCONN);

	// Warte auf Verbindung vom Client
	vector<thread> threads;
	for (int i = 0; i < 16; i++)
	{
		sockaddr_in client;
		int clientSize = sizeof(client);
		clientSocket[i] = accept(listening, (sockaddr*)&client, &clientSize);

		cout << "Client " << i << " ist verbunden !" << endl;

		threads.emplace_back(clientsThread, clientSocket[i], i);
	}
}

int main()
{
	thread getClientsThread(getClients);

	while (true)
	{
		string eingabe;
		int clientNumber = 0;
		getline(cin, eingabe);

	std:string delimiter = " ";

		size_t position = 0;
		string* command = new string[16];

		position = eingabe.find(delimiter);
		command[0] = eingabe.substr(0, position);
		eingabe.erase(0, position + delimiter.length());
		position = eingabe.find(delimiter);
		command[1] = eingabe.substr(0, position);
		eingabe.erase(0, position + delimiter.length());
		command[2] = eingabe;

		if (command[0] == "client" && eingabe.size() > 0)
		{
			clientNumber = stoi(command[1]);
			int sendResult = send(clientSocket[clientNumber], command[2].c_str(), command[2].size() + 1, 0);
		}
		else
		{
			for (int i = 0; i < 16; i++)
			{
				int sendResult = send(clientSocket[i], command[2].c_str(), command[2].size() + 1, 0);
			}
		}
	}
}