/////////////////////////////////////////////////////////////////
//Project Name	: MIP											/
//File Name		: api.c											/
//Author		: Arif Kursad KAVAS								/
//Revision		: 0.1 											/
//Date			: 17/04/08										/
//																/
//																/
//																/
//																/
/////////////////////////////////////////////////////////////////
#include <windows.h>
#include <stdio.h>
#include "Uart.h"
LRESULT CALLBACK WndProc(HWND hWnd, UINT message,
					WPARAM wParam, LPARAM lParam);
				
int OnButtonClick(HWND hWnd, LPARAM lParam);
int AddCommand(void);
int SendUartMessage(void);
int ShowEditBox(void);
int HideEditBox(void);
int *ComposeMessage(int *Msg, int Length);

int ShowButton(void);
int	TransmitMessage(int *Msg, int Length);
int GetListboxIndex(void);
int GetMessageLength(void);
int GetCrcMsg(void);
int ResetMessageBuffer(void);

#define nEditID			100
#define ID_BUTTON		101	
#define ID_LISTBOX		102 
#define ID_CHILD		103
#define ID_BUTTONSELECT	104
#define ID_BUTTONSEND	105
	


HWND hButtonSend;
HWND hButtonSelect;
HWND hListBox;
HWND hMsg[10];
//HWND hEdit;

char *MipCommandList[] = {"StartSession", "StopSession", "SetRGBValues", "GetRGBValues","IncrementRGBValues",
						  "DecrementRGBValues ","ResetRGBValues", "ProtectRGBValues","RestoreRGBValues",
						  "SourceSwitch","CopyWB_EEPROM","SelfTest","IRCommand", "SwitchToProgram",
						  "SwitchToDVBProgram", "Check Video","ActivateADCCalibration", "ReadEEPROM",
						  "WriteEEPROM","ReadI2C","WriteI2C","MainVolUp","MainVolMid", "MainVolDown",
						  "HPVolUp", "HPVolMid","HPVolDown", "BalanceUp","BalanceMid", "BalanceDown",
					      "OpenServiceMenu", "OpenAudioMenu","OpenDVBMenu", "CheckHDCP","Factory RESET",
						  "FactoryRESETForTest", "SwitchToTEXT","CloseTEXT", "SwitchToPIP","ClosePIP",
						  "SetPictureAspect <Aspect>", "PCAutoConfiguration","SwitchToMain",
					      "ActivateFREEZE", "OpenBALANCE","OpenCA","CloseCA", "SwitchToTM2"};

int ArgumentList[] = {0,0,3,2,3,3,0,0,0,1,2,0,1,1,1,0,0,3,3,4,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0};
int IntMsg[15];


int WINAPI	WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
					LPSTR lpszCmdParam, int ncmdShow)
{
	WNDCLASS wndClass;
	HWND hWnd;
	MSG message;
	
	InitUart();

	if(!hPrevInstance)
	{
		wndClass.style = CS_HREDRAW | CS_VREDRAW;
		wndClass.cbClsExtra = 0;
		wndClass.cbWndExtra = 0;
		wndClass.hInstance = hInstance;
		wndClass.hIcon = LoadIcon(NULL, IDI_QUESTION);
		wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
		wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		wndClass.lpszMenuName = NULL;
		wndClass.lpszClassName = "Generic";
		wndClass.lpfnWndProc = (WNDPROC)WndProc;
		if(!RegisterClass(&wndClass))
			return -1;
	}
	
	hWnd = CreateWindow("Generic", "Sample Windows",
						WS_OVERLAPPEDWINDOW|
						WS_MINIMIZEBOX|
						WS_MAXIMIZEBOX|
						WS_SYSMENU
						//WS_HSCROLL|
						/*WS_VSCROLL*/,
						50,
						50,
						600,
						300,
						NULL,
						NULL,
						hInstance,
						NULL);
	if(!hWnd)
		return -1;

	ShowWindow(hWnd, SW_RESTORE);
	UpdateWindow(hWnd);


	while(GetMessage(&message,0,0,0))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);
	}
	return message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch(message)
	{

		case WM_CREATE:

			hButtonSelect = CreateWindow("button", "Select", WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,
									250, 100, 100, 50, hWnd, (HMENU)ID_BUTTONSELECT, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

			hButtonSend = CreateWindow("button", "Send", WS_CHILD|BS_PUSHBUTTON,
									400, 100, 100, 50, hWnd, (HMENU)ID_BUTTONSEND, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

			hListBox = CreateWindow("listbox", "", WS_CHILD|WS_VISIBLE|LBS_NOTIFY|WS_HSCROLL|WS_VSCROLL,
									0, 0, 200, 200, hWnd, (HMENU)ID_LISTBOX, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

			hMsg[0] = CreateWindow( "edit", "Arg",WS_CHILD|WS_BORDER|ES_AUTOHSCROLL|ES_AUTOVSCROLL,	
									220, 10, 40, 20, hWnd, (HMENU)nEditID, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
	
			hMsg[1] = CreateWindow( "edit", "Arg",WS_CHILD|WS_BORDER|ES_AUTOHSCROLL|ES_AUTOVSCROLL,	
									280, 10, 40, 20, hWnd, (HMENU)nEditID, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

			hMsg[2] = CreateWindow( "edit", "Arg",WS_CHILD|WS_BORDER|ES_AUTOHSCROLL|ES_AUTOVSCROLL,	
									340, 10, 40, 20, hWnd, (HMENU)nEditID, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

			hMsg[3] = CreateWindow( "edit", "Arg",WS_CHILD|WS_BORDER|ES_AUTOHSCROLL|ES_AUTOVSCROLL,	
									400,10, 40, 20, hWnd, (HMENU)nEditID, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

			hMsg[4] = CreateWindow( "edit", "Arg",WS_CHILD|WS_BORDER|ES_AUTOHSCROLL|ES_AUTOVSCROLL,	
									460, 10, 40, 20, hWnd, (HMENU)nEditID, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
			AddCommand();
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_LBUTTONDOWN:
			break;
		case WM_RBUTTONDOWN:
			break;

		case WM_COMMAND:
			if(LOWORD(wParam) == ID_BUTTONSELECT && HIWORD(wParam) == BN_CLICKED)
			{
				OnButtonClick(hWnd, lParam);
			}
			if(LOWORD(wParam) == ID_BUTTONSEND && HIWORD(wParam) == BN_CLICKED)
			{
				SendUartMessage();
			}
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

int OnButtonClick(HWND hWnd, LPARAM lParam)
{
	HideEditBox();
	ShowEditBox();
	ShowButton();
	return 0;
}

int AddCommand(void)
{
	int i;

	for(i = 0; i < 48; i++)
	{
		SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)MipCommandList[i]);
	}
	return 0;
}
int SendUartMessage(void)
{
	int index = 0;
	int i = 0;
	char arg[4];
	int  Msg[5];
	
	index = GetListboxIndex();
	for(i = 0; i < ArgumentList[index]; i++)
	{ 
		GetWindowText(hMsg[i], arg, 4);
		Msg[i] = atoi(arg);
	}
	TransmitMessage(ComposeMessage(Msg,ArgumentList[index]), GetMessageLength());

	ResetMessageBuffer();
	HideEditBox();
	return 0;
}

int ShowEditBox(void)
{
	int index = 0;
	int i = 0;
		
	index = GetListboxIndex();
	for(i = 0; i < ArgumentList[index]; i++)
	{
		ShowWindow(hMsg[i], SW_SHOW);
	}
	return 0;
}

int HideEditBox(void)
{
	int i = 0;
	for(i = 0; i < 5; i++)
	{
		ShowWindow(hMsg[i], SW_HIDE);
	}
	return 0;
}

int *ComposeMessage(int *Msg, int Length)
{
	int i;

	IntMsg[0] = 224;
	IntMsg[1] = GetMessageLength();
	IntMsg[2] = GetListboxIndex() + 1;
	Length += 3;
	for(i = 3; i < Length; i++)
	{
		IntMsg[i] = Msg[i-3];
	}
	IntMsg[i] = GetCrcMsg();
	return IntMsg;
}

int ShowButton(void)
{
	ShowWindow(hButtonSend, SW_SHOW);
	return 0;
}

int GetListboxIndex(void)
{
	int index = 0;
	return index = SendMessage(hListBox,LB_GETCURSEL,0,0);

}

int GetMessageLength(void)
{
	int i = GetListboxIndex();
	return ArgumentList[i] + 4;
}

int GetCrcMsg(void)
{
	int i;
	int Crc = 0;
	for(i = 0; i < GetMessageLength()-1; i++)
	{
		Crc += IntMsg[i];
	}
	Crc = Crc & 0xFF;
	Crc = 255 - Crc;
	return Crc;
}

int ResetMessageBuffer(void)
{
	int i;

	for(i = 0; i < GetMessageLength(); i++)
		IntMsg[i] = 0;

	return 0;
}
