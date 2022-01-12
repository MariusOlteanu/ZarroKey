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

#include "UiMainWindow.hpp"

#include "resource.h"
#include "Version.h"

#include "HelperText.hpp"
#include "IoSymulator.hpp"

#include <string>
#include <stdexcept>

#include <commctrl.h>

#pragma comment(linker, \
    "/manifestdependency:\"type='win32' \
    name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
    processorArchitecture='*' \
    publicKeyToken='6595b64144ccf1df' language='*'\"")

#pragma comment(lib, "comctl32.lib")

UiMainWindow::UiMainWindow()
{
    this->font = ::CreateFontW(16, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Arial");
    this->fontText = ::CreateFontW(16, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Consolas");
    this->fontSmall = ::CreateFontW(14, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Consolas");

    this->backgroundColor = ::CreateSolidBrush(RGB(0x20, 0xA0, 0xC0));
    this->backgroundColorList = ::CreateSolidBrush(RGB(0x20, 0x90, 0xB0));
    this->transparentColor = ::CreateSolidBrush(UiMainWindow::TransparentColor);
}

void UiMainWindow::Run()
{
    MSG message = { };

    ::UpdateWindow(window);
    ::ShowWindow(window, SW_SHOW);

    this->init = true;

    while (::GetMessageW(&message, nullptr, 0, 0))
    {
        if (!::IsDialogMessageW(this->window, &message))
        {
            ::TranslateMessage(&message);
            ::DispatchMessageW(&message);
        }
    }
}

void UiMainWindow::UiRegisterClass(HINSTANCE _instance)
{
    this->instance = _instance;

    WNDCLASSEXW wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = UiMainWindow::MainWindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = this->instance;
    wcex.hIcon = ::LoadIconW(this->instance, MAKEINTRESOURCE(IDI_ZARROKEY));
    wcex.hCursor = ::LoadCursorW(nullptr, IDC_ARROW);
    wcex.hbrBackground = this->backgroundColor;
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = UiMainWindow::ZarroClassName;
    wcex.hIconSm = ::LoadIconW(this->instance, MAKEINTRESOURCE(IDI_ZARROKEY));

    ::RegisterClassExW(&wcex);
}

void UiMainWindow::CreateMainWindow()
{
    this->window = ::CreateWindowExW(WS_EX_LAYERED, ZarroClassName, ZarroWindowName, WS_POPUP | WS_VISIBLE, (::GetSystemMetrics(SM_CXSCREEN) - UiMainWindow::XWindowSize) / 2, 100, UiMainWindow::XWindowSize, UiMainWindow::YWindowSize, nullptr, nullptr, this->instance, nullptr);
    if (this->window == nullptr)
    {
        throw std::runtime_error("Failed to create the main window: " + std::to_string(::GetLastError()));
    }

    ::SendMessageW(
        ::CreateWindowExW(0, L"BUTTON", L"Info", WS_VISIBLE | WS_CHILD, 250, 78, 90, 20, window, (HMENU)(UINT_PTR)BUTTON_INFO_ID, instance, nullptr),
        WM_SETFONT, (WPARAM)(HFONT)this->fontSmall, (LPARAM)MAKELONG(FALSE, 0));

    ::SendMessageW(
        ::CreateWindowExW(0, L"BUTTON", L"About", WS_VISIBLE | WS_CHILD, 250, 100, 90, 20, window, (HMENU)(UINT_PTR)BUTTON_ABOUT_ID, instance, nullptr),
        WM_SETFONT, (WPARAM)(HFONT)this->fontSmall, (LPARAM)MAKELONG(FALSE, 0));

    ::SendMessageW(
        ::CreateWindowExW(0, L"STATIC", L"Made by Zarro (C) 2021", WS_VISIBLE | WS_CHILD | SS_NOTIFY, 7, 3, 160, 20, this->window, (HMENU)(UINT_PTR)COPYRIGHT_ID, this->instance, nullptr),
        WM_SETFONT, (WPARAM)(HFONT)this->fontSmall, (LPARAM)MAKELONG(FALSE, 0));

    ::SendMessageW(
        ::CreateWindowExW(0, L"STATIC", L"Profile name:", WS_VISIBLE | WS_CHILD, 150, 20, 130, 20, this->window, (HMENU)(UINT_PTR)-1, this->instance, nullptr),
        WM_SETFONT, (WPARAM)(HFONT)this->font, (LPARAM)MAKELONG(FALSE, 0));

    ::SendMessageW(
        ::CreateWindowExW(0, L"STATIC", L"Key 1", WS_VISIBLE | WS_CHILD, 3, 20, 130, 20, this->window, (HMENU)(UINT_PTR)-1, this->instance, nullptr),
        WM_SETFONT, (WPARAM)(HFONT)this->fontSmall, (LPARAM)MAKELONG(FALSE, 0));

    ::SendMessageW(
        ::CreateWindowExW(0, L"STATIC", L"Key 2", WS_VISIBLE | WS_CHILD, 3, 40, 130, 20, this->window, (HMENU)(UINT_PTR)-1, this->instance, nullptr),
        WM_SETFONT, (WPARAM)(HFONT)this->fontSmall, (LPARAM)MAKELONG(FALSE, 0));

    ::SendMessageW(
        ::CreateWindowExW(0, L"STATIC", L"Key 3", WS_VISIBLE | WS_CHILD, 3, 60, 130, 20, this->window, (HMENU)(UINT_PTR)-1, this->instance, nullptr),
        WM_SETFONT, (WPARAM)(HFONT)this->fontSmall, (LPARAM)MAKELONG(FALSE, 0));

    ::SendMessageW(
        ::CreateWindowExW(0, L"STATIC", L"Key 4", WS_VISIBLE | WS_CHILD, 3, 80, 130, 20, this->window, (HMENU)(UINT_PTR)-1, this->instance, nullptr),
        WM_SETFONT, (WPARAM)(HFONT)this->fontSmall, (LPARAM)MAKELONG(FALSE, 0));

    ::SendMessageW(
        ::CreateWindowExW(0, L"STATIC", L"Mouse L", WS_VISIBLE | WS_CHILD, 3, 100, 130, 20, this->window, (HMENU)(UINT_PTR)-1, this->instance, nullptr),
        WM_SETFONT, (WPARAM)(HFONT)this->fontSmall, (LPARAM)MAKELONG(FALSE, 0));

    ::SendMessageW(
        ::CreateWindowExW(0, L"STATIC", L"Mouse R", WS_VISIBLE | WS_CHILD, 3, 120, 130, 20, this->window, (HMENU)(UINT_PTR)-1, this->instance, nullptr),
        WM_SETFONT, (WPARAM)(HFONT)this->fontSmall, (LPARAM)MAKELONG(FALSE, 0));

    ::SendMessageW(
        ::CreateWindowExW(0, L"STATIC", L"Zarro key version " FILE_VERSION_STRING, WS_VISIBLE | WS_CHILD, 170, 122, 175, 20, this->window, (HMENU)(UINT_PTR)-1, this->instance, nullptr),
        WM_SETFONT, (WPARAM)(HFONT)font, (LPARAM)MAKELONG(FALSE, 0));

    ::CreateWindowExW(0, L"STATIC", L"", WS_VISIBLE | WS_CHILD, 165, 1, UiMainWindow::XWindowSize - 165 - 35, 18, this->window, (HMENU)(UINT_PTR)UiMainWindow::TRANSPARENT_ID, this->instance, nullptr);

    ::SendMessageW(
        ::CreateWindowExW(0, L"STATIC", L"", WS_CHILD | WS_VISIBLE | SS_ICON | SS_NOTIFY, UiMainWindow::XWindowSize - 35, 3, 32, 32, this->window, (HMENU)(UINT_PTR)PICTURE_ID, this->instance, nullptr),
        STM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)::LoadIconW(this->instance, MAKEINTRESOURCE(IDI_ZARROKEY)));

    ::CreateWindowExW(0, L"STATIC", L"", WS_VISIBLE | WS_CHILD | SS_ETCHEDVERT, 350, 21, 330, UiMainWindow::YWindowSize - 23, this->window, (HMENU)(UINT_PTR)-1, this->instance, nullptr);

    for (uint32_t index = 0; index < RegistryData::maxData; index++)
    {
        ::CreateWindowExW(0, L"BUTTON", L"", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 60, 20 + index * 20, 20, 20, this->window, (HMENU)(UINT_PTR)UiMainWindow::CHECKBOX_ID[index], this->instance, nullptr);
        ::CheckDlgButton(this->window, UiMainWindow::CHECKBOX_ID[index], this->registryData.checkbox[index]);
        this->symulator->SetButtonCheck(index, this->registryData.checkbox[index]);
    }

    for (uint32_t index = 0; index < RegistryData::maxData; index++)
    {
        this->edit[index] = ::CreateWindowExW(WS_EX_PALETTEWINDOW, L"EDIT", this->registryData.timeValue[index], WS_VISIBLE | WS_CHILD | WS_BORDER | WS_TABSTOP | ES_CENTER | ES_NUMBER, 80, 20 + index * 20, 60, 20, this->window, (HMENU)(UINT_PTR)EDITBOX_ID[index], this->instance, nullptr);
        ::SendMessageW(edit[index], EM_SETLIMITTEXT, RegistryData::timeValueMaxChars, 0);
        this->symulator->SetTimeWait(index, this->registryData.timeInt[index]);

        ::SendMessageW(this->edit[index], WM_SETFONT, (WPARAM)(HFONT)this->fontText, (LPARAM)MAKELONG(FALSE, 0));
    }

    this->editList = ::CreateWindowExW(WS_EX_PALETTEWINDOW, L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | WS_TABSTOP | ES_LEFT, 150, 40, 190, 20, this->window, (HMENU)(UINT_PTR)UiMainWindow::EDITBOX_LIST_ID, this->instance, nullptr);
    this->list = ::CreateWindowExW(0, L"LISTBOX", L"", WS_VISIBLE | WS_CHILD | LBS_NOTIFY | WS_TABSTOP, 358, 25, 182, 115, this->window, (HMENU)(UINT_PTR)UiMainWindow::LISTBOX_ID, this->instance, nullptr);

    ::SendMessageW(this->editList, EM_SETLIMITTEXT, RegistryData::profilesNameLength, 0);
    ::SendMessageW(this->editList, WM_SETTEXT, 0, (LPARAM)this->registryData.GetProfileName(this->registryData.GetCurrentSelection()));

    for (uint32_t index = 0; index < RegistryData::profilesCount; index++)
    {
        ::SendMessageW(this->list, LB_ADDSTRING, 0, (LPARAM)this->registryData.GetProfileName(index));
    }

    ::SendMessageW(this->list, LB_SETCURSEL, this->registryData.GetCurrentSelection(), 0);

    ::SendMessageW(this->editList, WM_SETFONT, (WPARAM)(HFONT)this->font, (LPARAM)MAKELONG(FALSE, 0));
    ::SendMessageW(this->list, WM_SETFONT, (WPARAM)(HFONT)this->font, (LPARAM)MAKELONG(FALSE, 0));
    ::SetFocus(this->list);

    ::SetLayeredWindowAttributes(this->window, UiMainWindow::TransparentColor, 0x00, LWA_COLORKEY);
}

void UiMainWindow::Update(uint32_t position)
{
    wchar_t text[RegistryData::timeValueMaxChars + 1];
    uint32_t time;

    ::SendMessageW(this->edit[position], WM_GETTEXT, RegistryData::timeValueMaxChars + 1, (LPARAM)text);
    text[RegistryData::timeValueMaxChars] = L'\0';

    uint32_t value = _wtoi(text);
    if (value < RegistryData::minTimeValue)
    {
        return;
    }

    time = value;
    this->registryData.timeInt[position] = value;

    this->symulator->SetTimeWait(position, time);

    this->registryData.Apply();

    this->symulator->SignalEvent();
}

void UiMainWindow::UpdateAll()
{
    wchar_t text[RegistryData::timeValueMaxChars + 1];
    uint32_t time[RegistryData::maxData];

    for (uint32_t index = 0; index < RegistryData::maxData; index++)
    {
        ::SendMessageW(this->edit[index], WM_GETTEXT, RegistryData::timeValueMaxChars + 1, (LPARAM)text);
        text[RegistryData::timeValueMaxChars] = L'\0';

        uint32_t value = _wtoi(text);
        if (value < RegistryData::minTimeValue)
        {
            value = RegistryData::minTimeValue;

            ::_itow_s(value, text, 10);
            ::SendMessageW(this->edit[index], WM_SETTEXT, 0, (LPARAM)text);
        }

        time[index] = value;
        this->registryData.timeInt[index] = value;
    }

    this->symulator->SetTimeWait(time);

    this->registryData.Apply();

    this->symulator->SignalEvent();
}

void UiMainWindow::UpdateProfileName()
{
    if (!this->init)
    {
        return;
    }

    LRESULT length;
    wchar_t nameProfile[RegistryData::profilesNameLength + 1];

    length = ::SendMessageW(this->editList, WM_GETTEXT, RegistryData::profilesNameLength, (LPARAM)nameProfile);
    if (length > 0)
    {
        nameProfile[RegistryData::profilesNameLength] = L'\0';
        if (this->registryData.ChangeProfileName(nameProfile))
        {
            int32_t selection = this->registryData.GetCurrentSelection();
            ::SendMessageW(this->list, WM_SETREDRAW, FALSE, 0);
            ::SendMessageW(this->list, LB_DELETESTRING, selection, 0);
            ::SendMessageW(this->list, LB_INSERTSTRING, selection, (LPARAM)(LPARAM)this->registryData.GetProfileName(selection));
            ::SendMessageW(this->list, LB_SETCURSEL, selection, 0);
            ::SendMessageW(this->list, WM_SETREDRAW, TRUE, 0);
        }
    }
}

LRESULT UiMainWindow::MainWindow(HWND _window, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
                case UiMainWindow::PICTURE_ID:
                    ::PostQuitMessage(0);
                    break;
                case UiMainWindow::CHECKBOX_ID[0]:
                case UiMainWindow::CHECKBOX_ID[1]:
                case UiMainWindow::CHECKBOX_ID[2]:
                case UiMainWindow::CHECKBOX_ID[3]:
                case UiMainWindow::CHECKBOX_ID[4]:
                case UiMainWindow::CHECKBOX_ID[5]:
                {
                    UINT checked = ::IsDlgButtonChecked(_window, LOWORD(wParam));
                    this->symulator->SetButtonCheck(LOWORD(wParam) - UiMainWindow::CHECKBOX_ID[0], checked != 0);
                    this->registryData.checkbox[LOWORD(wParam) - UiMainWindow::CHECKBOX_ID[0]] = checked != 0;
                    this->registryData.Apply();
                    break;
                }
                case UiMainWindow::BUTTON_INFO_ID:
                {
                    this->UpdateAll();

                    HWND focus = ::GetFocus();
                    ::MessageBoxW(_window, InfoText, AppTitle, MB_OK);
                    ::SetFocus(focus);
                    break;
                }
                case UiMainWindow::BUTTON_ABOUT_ID:
                case UiMainWindow::COPYRIGHT_ID:
                {
                    this->UpdateAll();

                    HWND focus = ::GetFocus();
                    ::MessageBoxW(_window, AboutText, AppTitle, MB_OK);
                    ::SetFocus(focus);
                    break;
                }
                case UiMainWindow::LISTBOX_ID:
                    if (HIWORD(wParam) == LBN_SELCHANGE)
                    {
                        int32_t selection = (int32_t)::SendMessageW(list, LB_GETCURSEL, 0, 0);
                        if (selection >= 0)
                        {
                            this->registryData.SetCurrentSelection(selection);
                            ::SendMessageW(editList, WM_SETTEXT, 0, (LPARAM)this->registryData.GetProfileName(this->registryData.GetCurrentSelection()));

                            for (uint32_t index = 0; index < RegistryData::maxData; index++)
                            {
                                ::CheckDlgButton(_window, CHECKBOX_ID[index], this->registryData.checkbox[index]);
                                this->symulator->SetButtonCheck(index, this->registryData.checkbox[index]);

                                ::SendMessageW(this->edit[index], WM_SETTEXT, 0, (LPARAM)this->registryData.timeValue[index]);
                                this->symulator->SetTimeWait(index, this->registryData.timeInt[index]);
                            }
                        }
                    }
                    break;
                case UiMainWindow::EDITBOX_LIST_ID:
                    if (HIWORD(wParam) == EN_CHANGE)
                    {
                        this->UpdateProfileName();
                    }
                    break;
                case UiMainWindow::EDITBOX_ID[0]:
                case UiMainWindow::EDITBOX_ID[1]:
                case UiMainWindow::EDITBOX_ID[2]:
                case UiMainWindow::EDITBOX_ID[3]:
                case UiMainWindow::EDITBOX_ID[4]:
                case UiMainWindow::EDITBOX_ID[5]:
                    if (HIWORD(wParam) == EN_CHANGE)
                    {
                        this->Update(LOWORD(wParam) - UiMainWindow::EDITBOX_ID[0]);
                    }
                    break;
            }
            break;
        case WM_LBUTTONDOWN:
            this->dragWindow = true;
            this->clickedPosition.x = (int32_t)(int16_t)LOWORD(lParam);
            this->clickedPosition.y = (int32_t)(int16_t)HIWORD(lParam);

            ::SetCapture(_window);
            break;
        case WM_LBUTTONUP:
            ::ReleaseCapture();
            dragWindow = false;
            break;
        case WM_MOUSEMOVE:
            if (dragWindow == true)
            {
                POINT position;
                position.x = (int32_t)(int16_t)LOWORD(lParam);
                position.y = (int32_t)(int16_t)HIWORD(lParam);

                ::ClientToScreen(_window, &position);
                ::MoveWindow(_window, position.x - this->clickedPosition.x, position.y - this->clickedPosition.y, UiMainWindow::XWindowSize, UiMainWindow::YWindowSize, TRUE);
            }
            break;
        case WM_CTLCOLORBTN:
        {
            HDC hdcButton = (HDC)wParam;
            UINT id = ::GetWindowLongW((HWND)lParam, GWL_ID);

            ::SetTextColor(hdcButton, RGB(0xF0, 0x20, 0x20));
            ::SetBkColor(hdcButton, RGB(0x20, 0xA0, 0xC0));

            return (INT_PTR)(HBRUSH)backgroundColor;
        }
        case WM_CTLCOLOREDIT:
        {
            UINT id = ::GetWindowLongW((HWND)lParam, GWL_ID);
            if (id >= CHECKBOX_ID[0] && id <= CHECKBOX_ID[5])
            {
                break;
            }

            HDC hdcEdit = (HDC)wParam;
            ::SetTextColor(hdcEdit, RGB(0xFF, 0xFF, 0xFF));
            ::SetBkColor(hdcEdit, RGB(0x50, 0x50, 0xB0));
            return (INT_PTR)(HBRUSH)backgroundColorList;
        }
        case WM_CTLCOLORLISTBOX:
        {
            HDC hdcList = (HDC)wParam;
            ::SetTextColor(hdcList, RGB(0xF0, 0xF0, 0xF0));
            ::SetBkColor(hdcList, RGB(0x90, 0x90, 0xA0));
            return (INT_PTR)(HBRUSH)backgroundColorList;
        }
        case WM_CTLCOLORSTATIC:
        {
            HDC hdcStatic = (HDC)wParam;
            UINT id = ::GetWindowLongW((HWND)lParam, GWL_ID);
            switch (id)
            {
                case COPYRIGHT_ID:
                    ::SetTextColor(hdcStatic, RGB(0xF0, 0x20, 0x20));
                    break;
                case TRANSPARENT_ID:
                    ::SetBkColor(hdcStatic, TransparentColor);
                    return (INT_PTR)(HBRUSH)transparentColor;
                default:
                    ::SetTextColor(hdcStatic, RGB(0xF0, 0xFF, 0xF0));
                    break;
            }

            ::SetBkColor(hdcStatic, RGB(0x20, 0xA0, 0xC0));
            return (INT_PTR)(HBRUSH)backgroundColor;
        }
        case WM_DESTROY:
            this->symulator->SignalEvent();
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(_window, message, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK UiMainWindow::MainWindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    return UiMainWindow::mainWindow->MainWindow(window, message, wParam, lParam);
}
