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

#include "Utils.hpp"

#include "RegistryData.hpp"

class IoSymulator;

class UiMainWindow
{
public:

    static UiMainWindow* CreateMainWindow(HINSTANCE instance, IoSymulator* _symulator, const wchar_t* registryPath)
    {
        static UiMainWindow window;

        UiMainWindow::mainWindow = &window;

        UiMainWindow::mainWindow->symulator = _symulator;
        UiMainWindow::mainWindow->UiRegisterClass(instance);
        UiMainWindow::mainWindow->registryData.Init(registryPath);
        UiMainWindow::mainWindow->CreateMainWindow();

        return UiMainWindow::mainWindow;
    }

    constexpr operator HWND() const noexcept { return this->window; }

    void Run();

protected:

    UiMainWindow();

    void Update(uint32_t position);
    void UpdateAll();
    void UpdateProfileName();

    RegistryData registryData;

    windows_ptr<HFONT, decltype(&::DeleteObject)> font { ::DeleteObject };
    windows_ptr<HFONT, decltype(&::DeleteObject)> fontText { ::DeleteObject };
    windows_ptr<HFONT, decltype(&::DeleteObject)> fontSmall { ::DeleteObject };

    windows_ptr<HBRUSH, decltype(&::DeleteObject)> backgroundColor { ::DeleteObject };
    windows_ptr<HBRUSH, decltype(&::DeleteObject)> backgroundColorList { ::DeleteObject };
    windows_ptr<HBRUSH, decltype(&::DeleteObject)> transparentColor { ::DeleteObject };

    IoSymulator* symulator;

    HINSTANCE instance;

    HWND window;

    HWND list;
    HWND editList;
    HWND edit[RegistryData::maxData];

    bool init = false;

    inline static UiMainWindow* mainWindow = nullptr;

private:

    void UiRegisterClass(HINSTANCE _instance);
    void CreateMainWindow();

    __forceinline LRESULT MainWindow(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK MainWindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam);

    bool dragWindow = false;

    POINT clickedPosition;

    constexpr static uint32_t BUTTON_INFO_ID = 1;
    constexpr static uint32_t BUTTON_ABOUT_ID = 2;

    constexpr static uint32_t PICTURE_ID = 5;
    constexpr static uint32_t COPYRIGHT_ID = 10;
    constexpr static uint32_t TRANSPARENT_ID = 11;
    constexpr static uint32_t LISTBOX_ID = 15;
    constexpr static uint32_t EDITBOX_LIST_ID = 16;
    constexpr static uint32_t EDITBOX_ID[RegistryData::maxData] =
    {
        21,
        22,
        23,
        24,
        25,
        26
    };

    constexpr static uint32_t CHECKBOX_ID[RegistryData::maxData] =
    {
        31,
        32,
        33,
        34,
        35,
        36
    };

    constexpr static wchar_t ZarroWindowName[] = L"Zarro Key Symulater";
    constexpr static wchar_t ZarroClassName[] = L"ZarroClass";

    constexpr static COLORREF TransparentColor = RGB(0x41, 0x51, 0x61);

    constexpr static int32_t XWindowSize = 575;
    constexpr static int32_t YWindowSize = 142;
};
