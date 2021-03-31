#include<stdio.h>
#include <stdlib.h>
#include<winsock.h>
#include<iostream> //
#include<fstream> //
#include<string>
#include <cstdlib>//

#pragma warning(disable:4996) 
//����ũ�� ����efds
#define BUFSIZE 4096
#define _WINSOCK_DEPREATED_NO_WARNINGS
#pragma comment(lib,"Ws2_32.lib")
//#define user_ip "192.168.0.80 " //�׽�Ʈ������ define����� ����
//#define user_Port 1234 //�׽�Ʈ������ define����� ����

enum AdPos : int {
	top    = 0,
	left   = 1,
	right  = 2,
	bottom = 3
};
struct Position { int x, y; };

class Adopter {
public:
	Adopter(Position window_potion, Position window_size, HINSTANCE hInstance); //������
	void attach(int user_id); //id �� ����
	void detach(); //���ֱ�
	void run(WPARAM wParam, int pos); //����
	void is_show(bool is_show) { is_show ? ShowWindow(hwnd_, SW_SHOW) : ShowWindow(hwnd_, SW_HIDE); }
	HWND& get_hwnd() { return hwnd_; }
private:
	HWND hwnd_;
	int user_id_;
};

class FileShareWindow //singleton class
{ 
public:
	static FileShareWindow& get_instance(HINSTANCE hInstance = NULL);
	void add_user(int user_id, AdPos pos);
	void detach(AdPos pos);
	void run(HWND hWnd, WPARAM wParam);
	~FileShareWindow();
private:
	FileShareWindow(HINSTANCE hInstance);
	int system_screen_width_;
	int system_screen_height_;
	Adopter adopters[4];
};


class FileTransfer
{
public:
	FileTransfer(SOCKET clientsock, sockaddr_in sockinfo, int pos) {
		// �ʱ�ȭ �����ִ� �۾��� ���⼭��

		//ip�ּ� �ʱ�ȭ �ڹٿ��� ���� �����ϸ� �װ� �о���°� //////���ܻ� �ҽ� �ִ� �������ٰ� ���� �����س��� �׽�Ʈ
		char user_ip[13];

		FILE* filefp = fopen("UserIP.txt", "r"); //�����̸� ���� �� ���ֱ�
		fgets(user_ip, sizeof(user_ip), filefp);
		fclose(filefp);
		
		//��Ʈ��ȣ�� �׳� ���� ���� (��ǻ�� �ٸ��״ϱ�) �ٸ��� ����
		int user_Port;
		if(pos == 0) user_Port = 1234; //top���� ������ ��ǻ�� ��Ʈ��ȣ�� 1234�� ����
		else if (pos == 1) user_Port = 2345; //left
		else if (pos == 2) user_Port = 3456; //right
		else user_Port = 4567; //�̰Ŵ� ��¥ else�� bottom�����ϱ�.. ������ �����Ͱ��� 
	
		sockinfo.sin_port = htons(user_Port); //��Ʈ��ȣ ���� (�������忡�� Ŭ���̾�Ʈ�� ������ �� IP�� ��Ʈ��ȣ�� ���� ������)
		sockinfo.sin_addr.s_addr = inet_addr(user_ip); //IP�ּ� ����

		//������ ���� �õ�
		if (connect(clientsock, (SOCKADDR*)& sockinfo, sizeof(sockinfo)) == SOCKET_ERROR)
			printf(" ���� ���� ���� ");
		if (connect(clientsock, (SOCKADDR*)& sockinfo, sizeof(sockinfo)))
			printf("������ ���� ����!\n");
	}

	void file_go(SOCKET clientsock, TCHAR *szPathName) {
		// ���⿡ ���������ϴ� �Լ� ���� �ݺ��� ������ �־ �ϴ°�
		
		//���� ���� ����
		char myFile[256];
		WideCharToMultiByte(CP_ACP, 0, szPathName, MAX_PATH, myFile, 256, NULL, NULL);

		//���� ����
		FILE* fp;
		fp = fopen(myFile, "rb");
		//ifstream fileopen("");
		char filename[256];
		//strcpy_s(filename, sizeof(filename) ,myFile);
		ZeroMemory(filename, 256);
		sprintf(filename, myFile);

		// ���⼭ �����̸� �������ֱ� �����ο��� �ں��� �о \ ǥ�� ������ �� �ڸ� ������ �д´�.
		char realFileName[256]; //����.mp4�� ���� ���� ���� �̸��� �����Ϸ��� ���� ������ 
		int indexCheck=0;
		for (int i = 255; i >= 0; i--) {
			if (filename[i] == '\\') {
				indexCheck = i;
				break;
			}
		}
		int k = 0;
		while (1){
			indexCheck++;
			realFileName[k] = filename[indexCheck];
			k++;
			if (filename[indexCheck] == '\0') break;
		}
		
		//�������� �����̸��� ����
		send(clientsock, realFileName, 256, 0); //���⼭�� �ϸ� 
		//���� ũ�� ���
		fseek(fp, 0, SEEK_END);

		int totalbytes = ftell(fp);
		//���� ũ�� �������� ������
		send(clientsock, (char*)& totalbytes, sizeof(totalbytes), 0); 
		

		//���� ������ ���ۿ� ����� ����
		char buf[BUFSIZE];
		int numread;
		int numtotal = 0;
		//���� �����͸� ���� ������ �̵�
		rewind(fp);

		//�ݺ������� ���� ������ ������
		while (1) {
			//������ ������ ���ۿ� ����
			numread = fread(buf, 1, BUFSIZE, fp);
			//���� �����Ͱ� �����̶� �������
			if (numread > 0) {
				send(clientsock, buf, numread, 0);
				numtotal = numread;
			}
			//������ ��� ������ ���/
			else if (numread == 0 && numtotal == totalbytes) {
				printf("�� %d����Ʈ ���� ������ �Ϸ��߽��ϴ�.\n", numtotal);
				break;
			}
		}
		fclose(fp);
		// �������
		closesocket(clientsock);
		printf("������ ���� ����");
		//��������
		WSACleanup();
	}
private:
	int strlen;
	FILE* fp;
	char myFile[256];
	char filename[256];

};