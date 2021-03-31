#include <stdio.h>
#include <WinSock2.h>
#include <stdlib.h>

//����ũ�� ����
#define BUFSIZE 4096
#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "ws2_32.lib")

int dataReceive(SOCKET s, char* buf, int len, int flags);

int main(int argc, char* argv[]) {
	//������ ũ�⸦ ���� ����
	int retval;

	SOCKET sock, clientsock;
	WSADATA wsa;
	struct sockaddr_in sockinfo, clientinfo;
	int clientsize;
	char message[] = "success";
	char buf[BUFSIZE];

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		printf("�ʱ�ȭ ����\n");

	//����� ���� ���� �� �ʱ�ȭ.
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (sock == INVALID_SOCKET)
		printf("���� ���� ����\n");
	//�ҹ��ε� ó�� ���� �ʱ�ȭ
	ZeroMemory(&sockinfo, sizeof(sockinfo));
	sockinfo.sin_family = AF_INET;
	sockinfo.sin_port = htons(1234);
	sockinfo.sin_addr.s_addr = htonl(INADDR_ANY);

	//���ε�
	if (bind(sock, (SOCKADDR*)& sockinfo, sizeof(sockinfo)) == SOCKET_ERROR)
		printf(" bind ���� ");
	//�����Լ� ����
	if (listen(sock, SOMAXCONN) == SOCKET_ERROR)
		printf(" ��⿭ ���� ");

	//�ݺ������� Ŭ���̾�Ʈ�κ��� ���� ��û �ޱ�

	while (1) {
		clientsize = sizeof(clientinfo);

		printf("Ŭ���̾�Ʈ�κ��� ������ ��ٸ��� �ֽ��ϴ�...\n");

		clientsock = accept(sock, (SOCKADDR*)& clientinfo, &clientsize);

		if (clientsock == INVALID_SOCKET)
			printf(" Ŭ���̾�Ʈ ���� ���� ���� ");
		printf("Ŭ���̾�Ʈ ����");
		//���� �̸� �ޱ�
		char filename[256]= "URF4C2.jpg";
		ZeroMemory(filename, 256);
		dataReceive(clientsock, filename, 256, 0);
		printf("���� ���� �̸�: %s\n", filename);
		int totalbytes; //���� ������ ũ���
		dataReceive(clientsock, (char*)& totalbytes, sizeof(totalbytes), 0);
		printf("���� ���� ũ��:%d ����Ʈ\n", totalbytes);

		//file opens
		errno_t err; //�̰� �֛��������� �� �˾ƺ��� ��ﵵ�ȳ���.
		FILE* fp;
	//	fp = fopen(filename, "wb");

		err = fopen_s(&fp, filename, "wb"); //���⼭ �� �ȵǳ� fp = fopen (filename, "wb") �� �ϸ� ��  �� �ΰ����°� ���� ������
		if (err == 0) //err�� �ٺ��� ���ʵ�?
			puts("���Ͽ��� ����!\n");
		else {
			puts("���� ���� ����\n");
			return -1;
		}

		int numtotal = 0;
		while (1) {
			retval = dataReceive(clientsock, buf, BUFSIZE, 0);
			//�� ���� �����Ͱ� ������
			if (retval == 0) break;
			else {
				fwrite(buf, 1, retval, fp); //////fp�� null �̶� �ȵǴ�����
				//���� ������ ũ�⸸ŭ ������ ������
				numtotal += retval;
			}
		}
		//���۰��
		if (numtotal == totalbytes)
			printf("������ ���������� �޾ҽ��ϴ�.\n");
		else printf("������ ����� ���� ���߽��ϴ�.\n");
		//�ش� Ŭ���̾�Ʈ ���� ���
		closesocket(clientsock);
		printf("Ŭ���̾�Ʈ ����");
	
	}
	//send(clientsock, message, sizeof(message), 0);
	closesocket(sock);

	printf("����\n");

	WSACleanup();
}
int dataReceive(SOCKET s, char* buf, int len, int flags) {
	//��ſ� �ʿ��� ���� ����
	int received;
	char* ptr = buf;
	int left = len;
	//�����ִ� �����Ͱ� �ִ� ��� �ݺ������� ����
	while (left > 0) {
		received = recv(s, ptr, left, flags);
		//�� �̻� �����͸� �޾ƿ��� ���ϴ� ���
		if (received == 0) break;
		//�޾ƿ� �����Ͱ� �����ϴ� ���
		left -= received;
		ptr += received;
	}
	//�� ���� �� �ִµ����ͱ��̸� ��ȯ �����͸� �ѹ��̶� �޾����� 0�̾ƴѼ��� ��ȯ�ϰԵ�
	return (len - left);
}