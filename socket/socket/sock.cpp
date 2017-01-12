#include <winsock2.h>
#include <windows.h>
#include <iostream>
#pragma comment(lib, "ws2_32.lib")
#define BUFSIZE 1024

using namespace std;

int main() {
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		cout << "WSAStartup failed.\n";
		system("pause");
		return 1;
	}
	SOCKET Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	char URL[128];
	cout << "URL Input :";
	cin >> URL;
	struct hostent *host;
	if ((host = gethostbyname(URL)) == 0) {
		cout << "No signal";
		system("pause");
		return 1;
	}

	SOCKADDR_IN SockAddr;
	SockAddr.sin_port = htons(80);
	SockAddr.sin_family = AF_INET;
	SockAddr.sin_addr.s_addr = *((unsigned long*)host->h_addr);

	cout << "Connecting...\n";
	if (connect(Socket, (SOCKADDR*)(&SockAddr), sizeof(SockAddr)) != 0) {
		cout << "Could not connect" << endl;
		system("pause");
		return 1;
	}

	cout << "Connected.\n";
	char msg[BUFSIZE] = {0,};
	sprintf(msg, "GET / HTTP/1.1\r\nHost: %s\r\n\r\n", URL);
	send(Socket, (char *)msg , strlen(msg), 0);

	char buffer[BUFSIZE] = {0,};
	int length;

	while (1) {
		if ((length = recv(Socket, buffer, BUFSIZE, 0)) == 0) 
			break;
	}

	cout << buffer;
	memset(buffer, 0, BUFSIZE);
	closesocket(Socket);
	WSACleanup();
	system("pause");
	return 0;
}