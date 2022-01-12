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

#pragma once

#include <string>

#include <Windows.h>
#include <WinInet.h>

class Updater
{
public:

    ~Updater();

    bool Check(LPWSTR commandLine);

    void RunUpdater(HWND mainWindow);

protected:

    void Run();
    void ContinueUpdater();
    void PrintVersion();
    void Cleanup();

    bool Run(const std::wstring& processName, bool autoupdate = false);

    template <size_t DataSize>
    bool GetFiles(std::wstring& tempFile, const wchar_t (&fileName)[DataSize]);

private:

    HWND window = nullptr;

    HINTERNET webConnect = nullptr;
    HINTERNET webAddress = nullptr;

    DWORD Thread();

    static DWORD __stdcall ThreadCallback(Updater* updater);
};
