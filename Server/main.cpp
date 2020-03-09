#include <iostream>
#include <WS2tcpip.h>

#pragma comment (lib, "ws2_32.lib")

void main() {

	//Initialize WSDATA Object
	WSADATA wsData;
	//Using winsock version 2.2
	WORD ver = MAKEWORD(2, 2);

	//Call winsock API
	int wSok = WSAStartup(ver, &wsData);
	if (wSok != 0) {
		std::cerr << "Cant find winsock or winsock is not supported on this system. Exiting..." << std::endl;
		return;
	}

	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET) {
		std::cerr << "Cant create socket. Exiting..." << std::endl;
		WSACleanup();
		return;
	}

	sockaddr_in hint;
	//Specify IPv4 Address Family
	hint.sin_family = AF_INET;
	//Converts from little-endian to big-endian
	hint.sin_port = htons(54000);
	hint.sin_addr.S_un.S_addr = INADDR_ANY;

	bind(listening, (sockaddr*)&hint, sizeof(hint));

	listen(listening, SOMAXCONN);

	sockaddr_in client;
	int clientSize = sizeof(client);

	//Accept TCP connection
	SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize);
	if (clientSocket == INVALID_SOCKET) {
		std::cerr << "Server could not accept client connection %d\n", WSAGetLastError();
		closesocket(listening);
		WSACleanup();
		return;
	}

	char host[NI_MAXHOST];
	char service[NI_MAXSERV];

	ZeroMemory(host, NI_MAXHOST);
	ZeroMemory(service, NI_MAXSERV);

	//Get client information
	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0) {
		std::cout << host << " connected on port " << service << std::endl;
	}
	else {
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		std::cout << host << " connected on port " <<
			ntohs(client.sin_port) << std::endl;
	}

	closesocket(listening);

	char buf[4096];

	while (true) {
		ZeroMemory(buf, 4096);

		int bytesReceived = recv(clientSocket, buf, 4096, 0);
		if (bytesReceived == SOCKET_ERROR) {
			std::cerr << "Error in recv(). Exiting..." << std::endl;
			break;
		}

		if (bytesReceived == 0) {
			std::cout << "Client disconnected " << std::endl;
			break;
		}


		send(clientSocket, buf, bytesReceived + 1, 0);

	}

	//Shutdown socket
	closesocket(clientSocket);

	//Release WinSock DLL resources
	WSACleanup();

}

