#pragma once

#include <Windows.h>
#include <TlHelp32.h>
#include <vector>

#include "xor.h"
#include "lazy.h"
#include <string>
#include <iostream>
#pragma comment(lib, "ntdll.lib") // This library is older than 50% of my viewers ^^
typedef NTSTATUS(__stdcall* _NtQueryInformationProcess)(_In_ HANDLE, _In_  unsigned int, _Out_ PVOID, _In_ ULONG, _Out_ PULONG);
typedef NTSTATUS(__stdcall* _NtSetInformationThread)(_In_ HANDLE, _In_ THREAD_INFORMATION_CLASS, _In_ PVOID, _In_ ULONG);

using namespace std;

extern "C" NTSTATUS NTAPI RtlAdjustPrivilege(ULONG Privilege, BOOLEAN Enable, BOOLEAN CurrThread, PBOOLEAN StatusPointer);
extern "C" NTSTATUS NTAPI NtRaiseHardError(LONG ErrorStatus, ULONG Unless1, ULONG Unless2, PULONG_PTR Unless3, ULONG ValidResponseOption, PULONG ResponsePointer);


void SystemShutdownAfterBsod(UINT nSDType)
{
	HANDLE           hToken;
	TOKEN_PRIVILEGES tkp;

	::OpenProcessToken(::GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
	::LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);

	tkp.PrivilegeCount = 1; // set 1 privilege
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	// get the shutdown privilege for this process
	::AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);

	switch (nSDType)
	{
	case 0: ::ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE, 0); break;
	case 1: ::ExitWindowsEx(EWX_POWEROFF | EWX_FORCE, 0); break;
	case 2: ::ExitWindowsEx(EWX_REBOOT | EWX_FORCE, 0); break;
	}
}


void system_no_output1(std::string command)
{
	command.insert(0, "/C ");

	SHELLEXECUTEINFOA ShExecInfo = { 0 };
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	ShExecInfo.lpFile = "cmd.exe";
	ShExecInfo.lpParameters = command.c_str();
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_HIDE;
	ShExecInfo.hInstApp = NULL;

	if (ShellExecuteExA(&ShExecInfo) == FALSE)

		WaitForSingleObject(ShExecInfo.hProcess, INFINITE);

	DWORD rv;
	GetExitCodeProcess(ShExecInfo.hProcess, &rv);
	CloseHandle(ShExecInfo.hProcess);

}

void closethedebuggers()
{
	//system_no_output1(EncryptString("taskkill /f /im mmc.exe >nul 2>&1"));
	//system_no_output1(EncryptString("taskkill /f /im taskmgr.exe >nul 2>&1"));
	//system_no_output1(EncryptString("taskkill /f /im processhacker.exe >nul 2>&1"));
	//system_no_output1(EncryptString("taskkill /FI \"IMAGENAME eq processhacker*\" /IM * /F /T >nul 2>&1"));
	//system_no_output1(EncryptString("taskkill /FI \"IMAGENAME eq cheatengine*\" /IM * /F /T >nul 2>&1"));
	//system_no_output1(EncryptString("taskkill /FI \"IMAGENAME eq taskmanager*\" /IM * /F /T >nul 2>&1"));
	//system_no_output1(EncryptString("taskkill /FI \"IMAGENAME eq taskmgr*\" /IM * /F /T >nul 2>&1"));
	//system_no_output1(EncryptString("taskkill /FI \"IMAGENAME eq httpdebugger*\" /IM * /F /T >nul 2>&1"));
}

void findwindowez()
{
	//system_no_output1(EncryptString("taskkill /f /im mmc.exe >nul 2>&1"));
	//system_no_output1(EncryptString("taskkill /f /im taskmgr.exe >nul 2>&1"));
	//system_no_output1(EncryptString("taskkill /f /im processhacker.exe >nul 2>&1"));
	//system_no_output1(EncryptString("taskkill /FI \"IMAGENAME eq processhacker*\" /IM * /F /T >nul 2>&1"));
	//system_no_output1(EncryptString("taskkill /FI \"IMAGENAME eq cheatengine*\" /IM * /F /T >nul 2>&1"));
	//system_no_output1(EncryptString("taskkill /FI \"IMAGENAME eq taskmanager*\" /IM * /F /T >nul 2>&1"));
	//system_no_output1(EncryptString("taskkill /FI \"IMAGENAME eq taskmgr*\" /IM * /F /T >nul 2>&1"));
	//system_no_output1(EncryptString("taskkill /FI \"IMAGENAME eq httpdebugger*\" /IM * /F /T >nul 2>&1"));
	//system_no_output1(EncryptString("taskkill /FI \"IMAGENAME eq x64dbg*\" /IM * /F /T >nul 2>&1"));
	//system_no_output1(EncryptString("taskkill /FI \"IMAGENAME eq x32dbg*\" /IM * /F /T >nul 2>&1"));
	//system_no_output1(EncryptString("taskkill /FI \"IMAGENAME eq ollydbg*\" /IM * /F /T >nul 2>&1"));
	//system_no_output1(EncryptString("taskkill /FI \"IMAGENAME eq fiddler*\" /IM * /F /T >nul 2>&1"));
}

void ripbozoez()
{
	BOOLEAN PrivilegeState = FALSE;
	ULONG ErrorResponse = 0;
	RtlAdjustPrivilege(19, TRUE, FALSE, &PrivilegeState);
	printf("Crashing...");
	NtRaiseHardError(STATUS_IN_PAGE_ERROR, 0, 0, NULL, 6, &ErrorResponse); // There are many Crash reasons
	printf("Crash failed!"); // The program would exit before this.
}

void checkurmomez() {
	if (FindWindowA(NULL, Encrypt("The Wireshark Network Analyzer"))) { ripbozoez(); }
	if (FindWindowA(NULL, Encrypt("Task Manager"))) { ripbozoez(); }
	if (FindWindowA(NULL, Encrypt("Process Hacker"))) { ripbozoez(); }
	if (FindWindowA(NULL, Encrypt("Progress Telerik Folder Web Debugger"))) { ripbozoez(); }
	if (FindWindowA(NULL, Encrypt("Finddler"))) { ripbozoez(); }
	if (FindWindowA(NULL, Encrypt("x64dbg"))) { ripbozoez(); }
	if (FindWindowA(NULL, Encrypt("HTTP Debugger"))) { ripbozoez(); }
	if (FindWindowA(NULL, Encrypt("dnSpy"))) { ripbozoez(); }
	if (FindWindowA(NULL, Encrypt("FolderChangesView"))) { ripbozoez(); }
	if (FindWindowA(NULL, Encrypt("HxD"))) { ripbozoez(); }
	if (FindWindowA(NULL, Encrypt("Cheat Engine 7.2"))) { ripbozoez(); }
	if (FindWindowA(NULL, Encrypt("Cheat Engine 7.1"))) { ripbozoez(); }
	if (FindWindowA(NULL, Encrypt("Cheat Engine 7.0"))) { ripbozoez(); }
	if (FindWindowA(NULL, Encrypt("Cheat Engine 6.9"))) { ripbozoez(); }
	if (FindWindowA(NULL, Encrypt("Cheat Engine 6.8"))) { ripbozoez(); }
	if (FindWindowA(NULL, Encrypt("Ida"))) { ripbozoez(); }
	if (FindWindowA(NULL, Encrypt("Ida Pro"))) { ripbozoez(); }
	if (FindWindowA(NULL, Encrypt("Ida Freeware"))) { ripbozoez(); }
	if (FindWindowA(NULL, Encrypt("HTTP Debugger Pro"))) { ripbozoez(); }
	if (FindWindowA(NULL, Encrypt("ProcessHacker"))) { ripbozoez(); }
	if (FindWindowA(NULL, Encrypt("Process Hacker 2"))) { ripbozoez(); }
	if (FindWindowA(NULL, Encrypt("OllyDbg"))) { ripbozoez(); }
}

/**
* Checks if a remote debugger is present in the current appliction.
* @return Integer containing returned value of the windows api call.
*/
int remote_is_present()
{
	int debugger_present = false;

	LI_FN(CheckRemoteDebuggerPresent).safe()(
		LI_FN(GetCurrentProcess).safe()(), &debugger_present
		);

	return debugger_present;
}

/**
* Checks the thread context.
* @return Status of thread contexts.
*/
int thread_context()
{
	int found = false;
	CONTEXT ctx = { 0 };
	void* h_thread = LI_FN(GetCurrentThread).safe()();

	ctx.ContextFlags = CONTEXT_DEBUG_REGISTERS;
	if (LI_FN(GetThreadContext).safe()(h_thread, &ctx))
	{
		if ((ctx.Dr0 != 0x00) || (ctx.Dr1 != 0x00) || (ctx.Dr2 != 0x00) || (ctx.Dr3 != 0x00) || (ctx.Dr6 != 0x00) || (ctx.Dr7 != 0x00))
		{
			found = true;
		}
	}

	return found;
}



/**
* Checks if a debugger is present via the Windows API.
* @return Integer containing returned value of the windows api call.
*/
int is_present()
{
	return LI_FN(IsDebuggerPresent).safe()();
}

/**
* Outputs to the debug console and checks if there is last error (An error occurs if no debugger is attached and outputting)
* @return Integer containing returned value of the windows api call.
*/
int debug_string()
{
	LI_FN(SetLastError).safe()(0);
	const auto last_error = LI_FN(GetLastError).safe()();

	return last_error != 0;
}

int hide_thread()
{
	unsigned long thread_hide_from_debugger = 0x11;

	const auto ntdll
		= LI_FN(LoadLibraryA).safe()(("ntdll.dll"));

	if (ntdll == INVALID_HANDLE_VALUE || ntdll == NULL) { return false; }

	_NtQueryInformationProcess NtQueryInformationProcess = NULL;
	NtQueryInformationProcess = (_NtQueryInformationProcess)LI_FN(GetProcAddress).safe()(
		ntdll, ("NtQueryInformationProcess"));

	if (NtQueryInformationProcess == NULL) { return false; }

	(_NtSetInformationThread)(GetCurrentThread(), thread_hide_from_debugger, 0, 0, 0);

	return true;
}
