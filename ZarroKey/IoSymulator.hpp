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

class IoSymulator
{
public:

    static IoSymulator* CreateSymulator()
    {
        static IoSymulator symulator;

        IoSymulator::ioSymulator = &symulator;

        return IoSymulator::ioSymulator;
    }

    constexpr void SetMainWindow(HWND _window) { this->window = _window; }
    constexpr void SetButtonCheck(uint32_t position, bool state) { this->buttonChecked[position] = state; }

    void SetTimeWait(uint32_t position, uint32_t time);
    void SetTimeWait(uint32_t (&time)[RegistryData::maxData]);

    void SignalEvent();

    void Start();

protected:

    IoSymulator();
    ~IoSymulator();

    windows_ptr<> waitEvent;

    CRITICAL_SECTION lock;

    HWND window = NULL;

    float screen;

    LARGE_INTEGER frequency;

    inline static std::atomic_bool keyPressedByUs = false;

    std::atomic_bool destroyItems = false;
    std::atomic_bool mouseMiddleButtonAlt = false;
    std::atomic_bool mouseMiddleButtonCtrl = false;

    std::atomic_int mouseX = 0;
    std::atomic_int mouseY = 0;
    std::atomic_int mouseWheel = 0;

    bool pause = false;

    uint32_t timeWait[RegistryData::maxData] = { 0 };
    int64_t timeLast[RegistryData::maxData] = { 0 };

    bool run[RegistryData::maxData] = { false, false, false, false, false, false };
    bool buttonChecked[RegistryData::maxData] = { false, false, false, false, false, false };

    inline static IoSymulator* ioSymulator = nullptr;

    windows_ptr<HHOOK, decltype(&::UnhookWindowsHookEx)> hookMouse { ::UnhookWindowsHookEx };
    windows_ptr<HHOOK, decltype(&::UnhookWindowsHookEx)> hookKeyboard { ::UnhookWindowsHookEx };

    constexpr static int32_t precision = 40;
    constexpr static int32_t keyDelay = 50;
    constexpr static int32_t keyRandom = 30;

    __forceinline void Click(INPUT (&input)[2]);
    __forceinline void Move(INPUT (&input)[1], int32_t positionX, int32_t positionY);
    __forceinline void MoveRelative(INPUT (&input)[1], int32_t positionX, int32_t positionY);
    __forceinline void MoveAndClick(INPUT (&input)[3], int32_t positionX, int32_t positionY);
    __forceinline void MoveAndClickRelative(INPUT (&input)[3], int32_t positionX, int32_t positionY);
    __forceinline void PressEnter(INPUT (&input)[2], DWORD wait = 10);
    __forceinline void PressShift(INPUT (&input)[1], DWORD flags = 0);
    void DestroyItems();

    __forceinline int64_t MyGetCurrentTime();

private:

    __forceinline LRESULT Keyboard(int32_t code, WPARAM wParam, LPARAM lParam);
    __declspec(dllexport) static LRESULT CALLBACK KeyboardEvent(int32_t code, WPARAM wParam, LPARAM lParam);

    __forceinline LRESULT CALLBACK Mouse(int32_t code, WPARAM wParam, LPARAM lParam);
    __declspec(dllexport) static LRESULT CALLBACK MouseEvent(int32_t code, WPARAM wParam, LPARAM lParam);

    __forceinline DWORD Thread();
    static DWORD __stdcall ThreadCallback(IoSymulator* ioSymulator);
};
