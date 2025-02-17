// SimpleTCPClient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"



bool parse_cmd(int argc, char* argv[], char* host, short* port);
void error_msg(const char*);
void exit_handler();

SOCKET client_socket;
int main(int argc, char* argv[])
{
	atexit(exit_handler);
	short port;
	char host[128] = "";
	bool parse_cmd_result = parse_cmd(argc, argv, host, &port);

	if (!parse_cmd_result || !host || !strlen(host))
	{
		printf("Invalid host or port. Usage %s -h host -p port\n", argv[0]);
		return -1;
	}

	WSADATA ws;
	if (WSAStartup(MAKEWORD(2, 2), &ws)) {
		error_msg("Error init of WinSock2");
		return -1;
	}

	client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (client_socket <= 0) {
		error_msg("Can't create socket");
		return -1;
	}

	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = inet_addr(host);

	//Connect to the server
	if (connect(client_socket, (sockaddr*)&server_addr, sizeof(sockaddr))) {
		char err_msg[128] = "";
		sprintf(err_msg, "Can't connect to the server %s:%d", host, port);
		error_msg(err_msg);
		return -1;
	}

	printf("Connection to the server %s:%d success\n", host, port);

	char msg[256] = "";
	printf("%s", "Enter msg:");
	//fgets(msg, sizeof(msg), stdin);
	scanf("%[^\n]s", msg);
	int sc = send(client_socket, msg, sizeof(msg), 0);
	if (sc <= 0) {
		char err_msg[128] = "";
		sprintf(err_msg, "Can't send data to the server %s:%d", host, port);
		error_msg(err_msg);
		return -1;
	}

	closesocket(client_socket);

	return 0;
}

bool parse_cmd(int argc, char* argv[], char* host, short* port)
{
	if (argc < 2) {
		return false;
	}

	char all_args[256] = "";

	for (int i = 1; i < argc; ++i) {
		strcat(all_args, argv[i]);
		strcat(all_args, " ");
	}

	const int count_vars = 2;
	const int host_buf_sz = 128;
	int tmp_ports[count_vars] = { -1, -1};
	char tmp_hosts[count_vars][host_buf_sz];
	for (int i = 0; i < count_vars; ++i) {
		memset(tmp_hosts[i], 0, host_buf_sz);
	}
	char* formats[count_vars] = { "-h %s -p %d", "-p %d -h %s"};

	int results[] = {
		sscanf(all_args, formats[0], tmp_hosts[0], &tmp_ports[0]) - 2,
		sscanf(all_args, formats[1], &tmp_ports[1], tmp_hosts[1]) - 2
	};

	for (int i = 0; i < sizeof(results) / sizeof(int); ++i) {
		if (!results[i]) {
			if (strlen(tmp_hosts[i]) > 0) {
				strcpy(host, tmp_hosts[i]);
			}
			if (tmp_ports[i] > 0) {
				*port = (short)tmp_ports[i];
				return true;
			}
		}
	}

	return false;

}

void error_msg(const char* msg) {
	printf("%s\n", msg);
}

void exit_handler()
{
	closesocket(client_socket);
	WSACleanup();
}
