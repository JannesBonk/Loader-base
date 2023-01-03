#include <Windows.h>
#include <iostream>
#include <string>
#include <format>
#include <filesystem>
#include <TlHelp32.h>
#include <algorithm>
#include <map>
#include <regex>
#include <thread>
#include <wininet.h>
#include<psapi.h>
//#define WINDOWS_IGNORE_PACKING_MISMATCH
#pragma comment(lib, "Wininet.lib")
#pragma comment(lib, "urlmon.lib")

using namespace std;
string namedll;
DWORD pid;
HANDLE process;
HWND hwndproc;
DWORD clientDLL;

//injection procces
bool inject(DWORD pid, const char* dll)
{
	char myDLL[MAX_PATH];
	GetFullPathNameA(dll, MAX_PATH, myDLL, 0);
	HANDLE hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, pid);
	LPVOID allocatedMem = VirtualAllocEx(hProcess, NULL, sizeof(myDLL), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	WriteProcessMemory(hProcess, allocatedMem, myDLL, sizeof(myDLL), NULL);
	CreateRemoteThread(hProcess, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibrary, allocatedMem, 0, 0);
	CloseHandle(hProcess);
	return TRUE;
}

//GetModule is used to get dlls from the program you are trying to inject, in this case we check for client.dll
DWORD GetModule(DWORD pid, const char* name)
{
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
	MODULEENTRY32A mEntry;
	mEntry.dwSize = sizeof(MODULEENTRY32A);
	do
	{
		if (!strcmp(mEntry.szModule, name))
		{
			CloseHandle(snapshot);
			return (DWORD)mEntry.modBaseAddr;
		}
	} while (Module32Next(snapshot, &mEntry));
}

void main() noexcept
{
	//optional
	//SetConsoleTitleA("You can set a console title");
	int menuValue;
	//optional
	//CreateDirectoryA("In here you can create a directory and maybe put ur dll here too", NULL);
	//Using URLDownloadToFileA we can download dll from a link to a directory, besides that if you keep the same link we can auto update the DLL, example is Github, Medusa.uno uses that
	URLDownloadToFileA(NULL, "You put your download link here", "Change this to where you want to download your dll", 0, NULL);	
	//Sleeping before showing makes loader/cmd look cooler
	Sleep(200);
	cout << "Injector made by youtube.com/JanesBonk or Jannes#7012" << endl;
	Sleep(50);
	cout << "[1] Inject" << endl;
	Sleep(50);
	cout << "[2] Exit"<< endl;
	Sleep(50);
	cout << "Option : ";
	cin >> menuValue;
	//After getting the menu value we can check if csgo is running or not by using the function FindWindow
	hwndproc = FindWindowA(0, "Counter-Strike: Global Offensive - Direct3D 9");
	//Here we get the PID ( process id ) 
	GetWindowThreadProcessId(hwndproc, &pid);
	//Opening the module to get client.dll needed for cheat to work
	process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	clientDLL = GetModule(pid, "client.dll");
	char url[256];
	strcat_s(url, "https://www.google.com");
	bool isconnect = InternetCheckConnectionA(url, FLAG_ICC_FORCE_CONNECTION, 0);
	if (menuValue == 1)
	{	
		if (!isconnect)
		{
			cout << "No internet connection detected! Cannot download DLL" << endl;
			system("pause");
			return;
		}
		//Check if csgo is running
		HWND status = FindWindowA(0, "Counter-Strike: Global Offensive - Direct3D 9");
		//Here we get the PID ( process id ) 
		GetWindowThreadProcessId(status, &pid);
		//Opening the module to get client.dll needed for cheat to work
		HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
		DWORD client = GetModule(pid, "client.dll");
		if (pid > 1)
		{
			inject(pid, "DLL path here");
			cout << "Cheat injcected succesfully! Press insert to open menu" << endl;
			system("pause");
		}
		else
		{
			//If csgo isnt runnign we use ShellExecuteA to run csgo
			ShellExecuteA(NULL, "open", "steam://rungameid/730", NULL, NULL, SW_SHOWNORMAL);
			while (true)
			{
				//Check if csgo is running
				HWND status = FindWindow(0, "Counter-Strike: Global Offensive - Direct3D 9");
				//Here we get the PID ( process id ) 
				GetWindowThreadProcessId(status, &pid);
				//Opening the module to get client.dll needed for cheat to work
				HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
				DWORD client = GetModule(pid, "client.dll");

				//If csgo is detected we break the operation of checking if csgo is running
				if (pid > 1)
					break;
			}
			//Sleeping 5.5 seconds so we can asure that the dll injects
			Sleep(5500);
			inject(pid, "DLL path here");
			cout << "Cheat injcected succesfully! Press insert to open menu" << endl;
			system("pause");
		}
	}
	//If user selected menu value 2 we can just return and make the loader close
	else if (menuValue == 2)
	{
		return;
	}
}
