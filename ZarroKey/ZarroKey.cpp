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

#include "ZarroKey.h"
#include "Updater.hpp"
#include "IoSymulator.hpp"
#include "UiMainWindow.hpp"
#include "RegistryData.hpp"

#include <iostream>

#include <stdint.h>

int32_t WINAPI wWinMain(_In_ HINSTANCE instance, _In_opt_ HINSTANCE prevInstance, _In_ LPWSTR commandLine, _In_ int32_t cmdShow)
{
    MSG message = { };

    UNREFERENCED_PARAMETER(prevInstance);
    UNREFERENCED_PARAMETER(commandLine);

    Updater updater;
    if (!updater.Check(commandLine))
    {
        return -1;
    }

    HANDLE mutex = ::CreateMutexW(nullptr, FALSE, L"ZarroKey_mutext");
    if (mutex == nullptr)
    {
        return -1;
    }

    if (::GetLastError() == ERROR_ALREADY_EXISTS)
    {
        goto exit_mutex;
    }

    try
    {
        IoSymulator* symulator = IoSymulator::CreateSymulator();
        UiMainWindow* mainWindow = UiMainWindow::CreateMainWindow(instance, symulator, L"Software\\Zarro\\ZarroKey");

        updater.RunUpdater(*mainWindow);

        symulator->SetMainWindow(*mainWindow);
        symulator->Start();

        mainWindow->Run();
    }
    catch (std::runtime_error& err)
    {
        std::cerr << err.what() << std::endl;
    }

exit_mutex:

    ::CloseHandle(mutex);

    return (int32_t)message.wParam;
}
