//
// Copyright(c) 2022 Marius Olteanu olteanu.marius@gmail.com
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this softwareand associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// 
// The above copyright noticeand this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "Updater.hpp"

#include "Version.h"

constexpr wchar_t File[] = L"ZarroKey.exe";
constexpr wchar_t TempFile[] = L"ZarroKey.exe_new";

#pragma comment(lib, "urlmon.lib")
#pragma comment(lib, "Wininet.lib")

Updater::~Updater()
{
    this->Cleanup();
}

bool Updater::Check(LPWSTR commandLine)
{
    if (commandLine != nullptr)
    {
        if (::wcscmp(commandLine, L"run") == 0)
        {
            this->Run();

            return false;
        }
        else if (::wcscmp(commandLine, L"autoupdate") == 0)
        {
            this->ContinueUpdater();

            return false;
        }
        else if (::wcsncmp(commandLine, L"printver", 8) == 0)
        {
            this->PrintVersion();

            return false;
        }
    }

    return true;
}

void Updater::RunUpdater(HWND mainWindow)
{
    this->window = mainWindow;

    HANDLE thread = ::CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(Updater::ThreadCallback), this, 0, nullptr);
    if (thread != nullptr)
    {
        ::CloseHandle(thread);
    }
}

void Updater::Run()
{
    std::wstring file;

    if (!this->GetFiles(file, File))
    {
        return;
    }

    if (!this->Run(file))
    {
        return;
    }
}

void Updater::ContinueUpdater()
{
    BOOL result;

    DWORD retry = 30;

    std::wstring file;
    std::wstring tempFile;

    if (!this->GetFiles(tempFile, TempFile) || !this->GetFiles(file, File))
    {
        return;
    }

    do
    {
        result = ::MoveFileExW(tempFile.c_str(), file.c_str(), MOVEFILE_REPLACE_EXISTING);

        ::Sleep(10);
    }
    while (result == FALSE && (retry--) != 0);

    if (result == FALSE)
    {
        ::MessageBoxW(this->window, L"Failed to update the file on disk!", L"Berila!", MB_OK);

        return;
    }

    if (!this->Run(file))
    {
        MessageBoxW(this->window, L"Failed to start the new version!", L"Berila", MB_OK);

        return;
    }
}

void Updater::PrintVersion()
{
    ::WriteFile(::GetStdHandle(STD_OUTPUT_HANDLE), &Version, sizeof(Version), NULL, NULL);
}

void Updater::Cleanup()
{
    if (this->webConnect != nullptr)
    {
        ::InternetCloseHandle(this->webConnect);
        this->webConnect = nullptr;
    }

    if (this->webAddress != nullptr)
    {
        ::InternetCloseHandle(this->webAddress);
        this->webAddress = nullptr;
    }
}

bool Updater::Run(const std::wstring& processName, bool autoupdate)
{
    std::wstring parameters = processName;

    if (autoupdate)
    {
        parameters += L" autoupdate";
    }

    STARTUPINFOW startupInfo = { 0 };
    PROCESS_INFORMATION processInformation;
    SECURITY_ATTRIBUTES securityAttributesThread;
    SECURITY_ATTRIBUTES securityAttributesProcess;

    startupInfo.cb = sizeof(startupInfo);

    securityAttributesThread.nLength = sizeof(securityAttributesThread);
    securityAttributesThread.lpSecurityDescriptor = nullptr;
    securityAttributesThread.bInheritHandle = FALSE;

    securityAttributesProcess.nLength = sizeof(securityAttributesProcess);
    securityAttributesProcess.lpSecurityDescriptor = nullptr;
    securityAttributesProcess.bInheritHandle = TRUE;

    BOOL result = ::CreateProcessW(nullptr, parameters.data(), &securityAttributesProcess, &securityAttributesThread, FALSE, 0, nullptr, nullptr, &startupInfo, &processInformation);
    if (result == FALSE)
    {
        return false;
    }

    ::CloseHandle(processInformation.hThread);
    ::CloseHandle(processInformation.hProcess);

    return true;
}

template <size_t DataSize>
bool Updater::GetFiles(std::wstring& tempFile, const wchar_t (&fileName)[DataSize])
{
    wchar_t currentFilePath[2048];

    DWORD bytesWritten = ::GetModuleFileNameW(nullptr, currentFilePath, _countof(currentFilePath));
    if (bytesWritten == 0)
    {
        return false;
    }

    if (::GetLastError() == ERROR_INSUFFICIENT_BUFFER)
    {
        return false;
    }

    wchar_t* directoryPath = ::wcsrchr(currentFilePath, L'\\');
    if (directoryPath == nullptr)
    {
        return false;
    }

    size_t sizeNeeded = directoryPath - currentFilePath + DataSize;
    if (sizeNeeded >= sizeof(currentFilePath))
    {
        return false;
    }

    *(++directoryPath) = '\0';
    ::memcpy(directoryPath, fileName, DataSize * 2);
    tempFile = currentFilePath;

    return true;
}

DWORD Updater::Thread()
{
    std::wstring tempFile;

    this->webConnect = ::InternetOpenW(L"ZarroKeyApp", INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0);
    if (this->webConnect == nullptr)
    {
        return (DWORD)-1;
    }

    this->webAddress = ::InternetOpenUrlW(this->webConnect, L"http://byte-it.in/zarrokey.ver", nullptr, 0, INTERNET_FLAG_KEEP_CONNECTION, 0);
    if (this->webAddress == nullptr)
    {
        return (DWORD)-2;
    }

    DWORD data;
    DWORD bytesRead;

    BOOL result = ::InternetReadFile(webAddress, &data, sizeof(data), &bytesRead);
    if (result == FALSE)
    {
        return 0;
    }

    if (data > Version)
    {
        if (IDYES != ::MessageBoxW(this->window, L"New version, update?", L"Berila software", MB_YESNO))
        {
            return (DWORD)-3;
        }
    }
    else
    {
        return 0;
    }

    if (!this->GetFiles(tempFile, TempFile))
    {
        return (DWORD)-4;
    }

    HRESULT hresult = ::URLDownloadToFileW(nullptr, L"http://byte-it.in/ZarroKey.exe", tempFile.c_str(), 0, nullptr);
    if (hresult != S_OK)
    {
        MessageBoxW(this->window, L"Failed to download the new version!", L"Berila", MB_OK);

        return (DWORD)-5;
    }

    if (!this->Run(tempFile, true))
    {
        MessageBoxW(this->window, L"Failed to update to the new version!", L"Berila", MB_OK);

        return (DWORD)-6;
    }

    ::exit(1);

    return 0;
}

DWORD __stdcall Updater::ThreadCallback(Updater* updater)
{
    return updater->Thread();
}
