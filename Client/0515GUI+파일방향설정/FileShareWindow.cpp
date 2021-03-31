#include "FileShareWindow.h"

Adopter::Adopter(Position window_potion, Position window_size, HINSTANCE hInstance) :user_id_(0) {
	hwnd_ = CreateWindow(L"Window Class Name", L"File Transfer", WS_POPUP, window_potion.x, window_potion.y, window_size.x, window_size.y, NULL, NULL, hInstance, NULL);
	DragAcceptFiles(hwnd_, TRUE);
	SetWindowLong(hwnd_, GWL_EXSTYLE, GetWindowLong(hwnd_, GWL_EXSTYLE) | WS_EX_LAYERED);
	SetLayeredWindowAttributes(hwnd_, 0, 130, LWA_ALPHA);
	is_show(false);
	UpdateWindow(hwnd_);
}

void Adopter::attach(int user_id)  { user_id_ = user_id; is_show(true); }  //user_id�Է¹������� 0�϶� is_show���ϰ� user+id private�� user_id�� ���ؼ� ������ is_show����
void Adopter::detach() { user_id_ = 0; is_show(false); }

void Adopter::run( WPARAM wParam, int pos) {
	TCHAR szPathName[MAX_PATH] = { '\0', }; //szPathName : ������ ��θ� ���� ����
	UINT num = DragQueryFile((HDROP)wParam, -1, 0, 0);  //�巡�� �� ������ ��θ� ��������
	for (UINT n = 0; n < num; ++n) {
		UINT result = DragQueryFile((HDROP)wParam, n, szPathName, MAX_PATH);
	}
	DragFinish((HDROP)wParam);  //�巡�� ��

	SOCKET clientsock; //���ϻ���
	WSADATA wsa; //�������α׷��� �Լ�������� ����
	struct sockaddr_in sockinfo;//���� �ּ� ����ü ����
	char message[30];
	int strlen;
	/*	if (argc != 3)
		{
			printf("Usage : %s <IP> <PORT> \n", argv[0]);
			//	exit(1);
		}*/

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)  //�������α׷��� �ʱ�ȭ
		printf("�ʱ�ȭ ����\n");

	clientsock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //�ּ�ü�� , ����Ÿ��(TCP/IP�� STREAM), ��������:TCP
	//���ͳ� ������ ����ϴ� TCP����� �������� ���� ���� =>clientsock
	if (clientsock == INVALID_SOCKET) //INVALID_SOCKET : �����ڵ尪
		printf("���� ���� ����\n");

	memset(&sockinfo, 0, sizeof(sockinfo));
	sockinfo.sin_family = AF_INET;

	//class ��ü���� ����� �Լ� ȣ��
	FileTransfer obj(clientsock,sockinfo,pos); //�����ڿ��� ��Ʈ�� ������ ��������..
	obj.file_go(clientsock,szPathName);

	//MessageBox(hwnd_, szPathName, (LPCWSTR)"a", MB_OK); // ���⿡ �������� �Լ� ȣ��
}

FileShareWindow& FileShareWindow::get_instance(HINSTANCE hInstance) {
	static FileShareWindow instance(hInstance);
	return instance;
}

FileShareWindow::FileShareWindow(HINSTANCE hInstance) 
	: system_screen_height_(GetSystemMetrics(SM_CXSCREEN))
	, system_screen_width_(GetSystemMetrics(SM_CYSCREEN))
	, adopters{
		   {{500,0}, {system_screen_width_*2-1250, 40}, hInstance}  //top
		 , {{0,200}, {40,system_screen_height_-1250}, hInstance} //left
		 , {{system_screen_width_ *2 - 280,200}, {40,system_screen_height_-1250}, hInstance} //right
		 , {{0,system_screen_height_-1000}, {system_screen_width_*2,40},  hInstance} //bottom
	}
{ }

void FileShareWindow::add_user(int user_id, AdPos pos) {
	adopters[pos].attach(user_id);
}

void FileShareWindow::detach(AdPos pos) {
	adopters[pos].detach();
}

void FileShareWindow::run(HWND hWnd, WPARAM wParam) {
	for (int i = 0; i < 4; ++i) {
		if (adopters[i].get_hwnd() == hWnd) adopters[i].run(wParam,i);
	}
}

FileShareWindow::~FileShareWindow() { }
