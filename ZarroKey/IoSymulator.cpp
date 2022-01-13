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

#include "IoSymulator.hpp"

#include <stdexcept>
#include <string>

IoSymulator::IoSymulator()
{
    ::InitializeCriticalSection(&this->lock);

    this->waitEvent = ::CreateEventW(nullptr, FALSE, FALSE, nullptr);

    ::QueryPerformanceFrequency(&this->frequency);
}

IoSymulator::~IoSymulator()
{
    if (hookMouse != nullptr)
    {
        ::UnhookWindowsHookEx(hookMouse);
    }

    if (hookKeyboard != nullptr)
    {
        ::UnhookWindowsHookEx(hookKeyboard);
    }

    ::DeleteCriticalSection(&lock);
}

void IoSymulator::SetTimeWait(uint32_t position, uint32_t time)
{
    ::EnterCriticalSection(&this->lock);
    this->timeWait[position] = time;
    ::LeaveCriticalSection(&this->lock);
}

void IoSymulator::SetTimeWait(uint32_t (&time)[RegistryData::maxData])
{
    ::EnterCriticalSection(&this->lock);
    for (uint32_t index = 0; index < RegistryData::maxData; index++)
    {
        this->timeWait[index] = time[index];
    }
    ::LeaveCriticalSection(&this->lock);
}

void IoSymulator::SignalEvent()
{
    ::SetEvent(this->waitEvent);
}

void IoSymulator::Start()
{
    windows_ptr thread = ::CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(IoSymulator::ThreadCallback), nullptr, 0, nullptr);

    HINSTANCE instance = ::GetModuleHandleW(nullptr);
    this->hookKeyboard = ::SetWindowsHookExW(WH_KEYBOARD_LL, static_cast<HOOKPROC>(IoSymulator::KeyboardEvent), instance, 0);

    // This is only for demo proposes, don't install it as it is. WH_MOUSE is the way to go, but then we need to place in into a dll,
    // which it will be injected into all the processes that we want to monitor
//    this->hookMouse = ::SetWindowsHookExW(WH_MOUSE_LL, (HOOKPROC)IoSymulator::MouseEvent, instance, 0);
}

void IoSymulator::Click(INPUT& input)
{
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP;
}

void IoSymulator::Move(INPUT& input, int32_t positionX, int32_t positionY)
{
    input.type = INPUT_MOUSE;
    input.mi.dx = positionX;
    input.mi.dy = positionY;
    input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
}

void IoSymulator::MoveRelative(INPUT& input, int32_t positionX, int32_t positionY)
{
    input.type = INPUT_MOUSE;
    input.mi.dx = (LONG)((this->screen * positionX) / ((float)1080 / 1920));
    input.mi.dy = positionY;
    input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
}

void IoSymulator::MoveAndClick(INPUT& input, int32_t positionX, int32_t positionY)
{
    input.type = INPUT_MOUSE;
    input.mi.dx = positionX;
    input.mi.dy = positionY;
    input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP;
}

void IoSymulator::MoveAndClick3(INPUT (&input)[3], int32_t positionX, int32_t positionY)
{
    for (int32_t index = 0; index < 2; index++)
    {
        input[0].type = INPUT_MOUSE;
        input[0].mi.dx = positionX - 10 + index * 5;
        input[0].mi.dy = positionY - 10 + index * 5;
        input[0].mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP;
    }
}

void IoSymulator::MoveAndClickRelative(INPUT& input, int32_t positionX, int32_t positionY)
{
    input.type = INPUT_MOUSE;
    input.mi.dx = (LONG)((this->screen * positionX) / ((float)1080 / 1920));
    input.mi.dy = positionY;
    input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP;
}

void IoSymulator::MoveAndClickRelative3(INPUT (&input)[3], int32_t positionX, int32_t positionY)
{
    for (int32_t index = 0; index < 2; index++)
    {
        input[0].type = INPUT_MOUSE;
        input[0].mi.dx = (LONG)((this->screen * (positionX - 10 + index * 5)) / ((float)1080 / 1920));
        input[0].mi.dy = positionY - 10 + index * 5;
        input[0].mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP;
    }
}

void IoSymulator::PressEnter(INPUT (&input)[2], DWORD wait)
{
    ::Sleep(wait);

    input[0].type = INPUT_KEYBOARD;
    input[0].ki.wVk = VK_RETURN;
    input[0].ki.dwFlags = 0;

    input[1].type = INPUT_KEYBOARD;
    input[1].ki.wVk = VK_RETURN;
    input[1].ki.dwFlags = KEYEVENTF_KEYUP;

    IoSymulator::keyPressedByUs = true;
    ::SendInput(ARRAYSIZE(input), input, sizeof(*input));
    IoSymulator::keyPressedByUs = false;
}

void IoSymulator::PressShift(INPUT& input, DWORD flags)
{
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = VK_SHIFT;
    input.ki.dwFlags = flags;
}

void IoSymulator::DestroyItems(bool repair)
{
    INPUT input[12] = { };

    int32_t screenX = ::GetSystemMetrics(SM_CXSCREEN) - 1;
    int32_t screenY = ::GetSystemMetrics(SM_CYSCREEN) - 1;

    uint32_t adjust = repair * 2;

    this->screen = (float)screenY / screenX;

    IoSymulator::keyPressedByUs = true;

#pragma warning( suppress: 6011 )
    this->PressShift(input[0]);

    if (repair)
    {
        this->MoveAndClickRelative(input[1], 17400, 37100);
        this->MoveAndClickRelative(input[2], 8900, 32400);
    }

    this->MoveAndClickRelative(input[1 + adjust], 17400, 29100);
    this->MoveAndClickRelative(input[2 + adjust], 13100, 17800);
    this->MoveAndClick(input[3 + adjust], 29000, 22500);

    this->MoveAndClickRelative(input[4 + adjust], 10880, 17800);
    this->MoveAndClick(input[5 + adjust], 29000, 22500);

    this->MoveAndClickRelative(input[6 + adjust], 8615, 17800);
    this->MoveAndClick(input[7 + adjust], 29000, 22500);

    POINT point;
    ::GetCursorPos(&point);

    int32_t pointX = point.x * 65535 / screenX;
    int32_t pointY = point.y * 65535 / screenY;

    this->Move(input[10 - adjust], pointX, pointY);

    this->PressShift(input[11 - adjust], KEYEVENTF_KEYUP);

    ::SendInput(ARRAYSIZE(input) - adjust, input, sizeof(*input));

    IoSymulator::keyPressedByUs = false;

    this->destroyItems = false;
}

void IoSymulator::DestroyAncient()
{
    INPUT input[7] = { };

    int32_t screenX = ::GetSystemMetrics(SM_CXSCREEN) - 1;
    int32_t screenY = ::GetSystemMetrics(SM_CYSCREEN) - 1;

    this->screen = (float)screenY / screenX;

    IoSymulator::keyPressedByUs = true;

    POINT point;
    ::GetCursorPos(&point);

    int32_t pointX = point.x * 65535 / screenX;
    int32_t pointY = point.y * 65535 / screenY;

    if ((65535 - pointX) < (int32_t)((this->screen * 18030) / ((float)1080 / 1920)) && pointY > 33200 && pointY < 52400)
    {
#pragma warning( suppress: 6011 )
        this->PressShift(input[0]);

        this->MoveAndClickRelative(input[1], 17400, 29100);

        this->MoveAndClick(input[2], pointX, pointY);
        this->MoveAndClickRelative(input[3], 5700, 17800);

        this->MoveAndClick(input[4], 29000, 22500);

        this->Move(input[5], pointX, pointY);

        this->PressShift(input[6], KEYEVENTF_KEYUP);

        ::SendInput(ARRAYSIZE(input), input, sizeof(*input));
    }

    IoSymulator::keyPressedByUs = false;
}

int64_t IoSymulator::MyGetCurrentTime()
{
    LARGE_INTEGER timeCurrent;

    ::QueryPerformanceCounter(&timeCurrent);
    timeCurrent.QuadPart = 1000 * timeCurrent.QuadPart / this->frequency.QuadPart;

    return timeCurrent.QuadPart;
}

LRESULT IoSymulator::Keyboard(int32_t code, WPARAM wParam, LPARAM lParam)
{
    if (wParam == WM_SYSKEYDOWN || wParam == WM_KEYDOWN)
    {
        KBDLLHOOKSTRUCT hookedKey = *((KBDLLHOOKSTRUCT*)lParam);
        int32_t key = hookedKey.vkCode;

        DWORD pressedCtrl = ::GetAsyncKeyState(VK_CONTROL);
        DWORD pressedAlt = ::GetAsyncKeyState(VK_MENU);

        if (key == 0x30 || key == VK_OEM_3)
        {
            this->mouseWheel = 0;

            if (pressedAlt || pressedCtrl)
            {
                int64_t currentTime = MyGetCurrentTime();

                ::EnterCriticalSection(&this->lock);
                for (uint32_t index = 0; index < _countof(this->buttonChecked); index++)
                {
                    if (pressedAlt)
                    {
                        this->run[index] = false;
                    }
                    else if (pressedCtrl && this->buttonChecked[index])
                    {
                        this->timeLast[index] = currentTime - this->timeWait[index];
                        this->run[index] = true;
                    }
                }

                this->pause = false;
                ::LeaveCriticalSection(&this->lock);
            }
            else
            {
                ::EnterCriticalSection(&this->lock);
                this->pause = !this->pause;
                ::LeaveCriticalSection(&this->lock);
            }

            ::SetEvent(waitEvent);
        }
        else if (key == VK_F2)
        {
            DWORD pressedCtrl = ::GetAsyncKeyState(VK_CONTROL);
            if (pressedCtrl)
            {
                this->DestroyItems(true);

                this->destroyItems = true;
                ::SetEvent(this->waitEvent);
            }
            else
            {
                this->DestroyAncient();
            }
        }
        else if (key == VK_F3)
        {
            this->mouseWheel = -50;
            ::SetEvent(this->waitEvent);
        }
        else if (key == VK_F4)
        {
            this->DestroyItems(true);

            this->destroyItems = true;
            ::SetEvent(this->waitEvent);
        }
        else if (key == VK_F5)
        {
            this->mouseWheel = 50;
            ::SetEvent(this->waitEvent);
        }
        else if (key == VK_F6)
        {
            this->mouseWheel = 10;
            ::SetEvent(this->waitEvent);
        }
        else if ((key >= 0x31 && key < 0x31 + RegistryData::maxData) && (pressedAlt || pressedCtrl))
        {
            ::EnterCriticalSection(&this->lock);
            if (pressedAlt)
            {
                this->run[key - 0x31] = false;
            }
            else if (pressedCtrl)
            {
                int64_t currentTime = MyGetCurrentTime();

                this->timeLast[key - 0x31] = currentTime - this->timeWait[key - 0x31];
                this->run[key - 0x31] = true;
            }

            this->pause = false;
            ::LeaveCriticalSection(&this->lock);
            ::SetEvent(this->waitEvent);
        }
    }

    return ::CallNextHookEx(nullptr, code, wParam, lParam);
}

LRESULT IoSymulator::Mouse(int32_t code, WPARAM wParam, LPARAM lParam)
{
    if (wParam == WM_MOUSEMOVE)
    {
        MSLLHOOKSTRUCT hookedMouse = *((MSLLHOOKSTRUCT*)lParam);

        this->mouseX.store(hookedMouse.pt.x, std::memory_order_relaxed);
        this->mouseY.store(hookedMouse.pt.y, std::memory_order_relaxed);

        return ::CallNextHookEx(nullptr, code, wParam, lParam);
    }
    else if (wParam == WM_MOUSEWHEEL)
    {
        DWORD pressedCtrl = ::GetAsyncKeyState(VK_CONTROL);
        DWORD pressedAlt = ::GetAsyncKeyState(VK_MENU);

        if (pressedCtrl || pressedAlt)
        {
            MSLLHOOKSTRUCT hookedMouse = *((MSLLHOOKSTRUCT*)lParam);

            int16_t delta = HIWORD(hookedMouse.mouseData);
            if (delta > 0)
            {
                if (pressedCtrl)
                {
                    this->mouseWheel += 50;
                }
                else
                {
                    this->mouseWheel += 10;
                }
            }
            else
            {
                if (pressedCtrl)
                {
                    this->mouseWheel -= 50;
                }
                else
                {
                    this->mouseWheel -= 10;
                }
            }

            ::SetEvent(this->waitEvent);
        }
    }
    else if (wParam == WM_MBUTTONDOWN)
    {
        DWORD pressedCtrl = ::GetAsyncKeyState(VK_CONTROL);
        DWORD pressedAlt = ::GetAsyncKeyState(VK_MENU);

        if (pressedCtrl || pressedAlt)
        {
            if (pressedCtrl)
            {
                this->mouseMiddleButtonCtrl = true;
            }

            if (pressedAlt)
            {
                this->mouseMiddleButtonAlt = true;
            }

            ::SetEvent(this->waitEvent);
        }
        else
        {
            this->DestroyItems(true);

            this->destroyItems = true;
            ::SetEvent(this->waitEvent);
        }
    }

    return ::CallNextHookEx(nullptr, code, wParam, lParam);
}

DWORD IoSymulator::Thread()
{
    uint32_t minWait = INFINITE;

    int64_t timeCurrent;

    while (true)
    {
        bool clickedOn[RegistryData::maxData + 1] = { false, false, false, false, false, false, false };

        DWORD result = ::WaitForSingleObject(this->waitEvent, minWait);
        timeCurrent = this->MyGetCurrentTime();
        minWait = INFINITE;

        if (window == ::GetForegroundWindow())
        {
            this->pause = true;

            continue;
        }

        if (destroyItems)
        {
            ::Sleep(50);
            this->DestroyItems();

            ::Sleep(50);
            this->DestroyItems();
        }

        if (this->mouseMiddleButtonCtrl)
        {
            INPUT input[2] = { };

            this->PressEnter(input, 50);

            this->mouseMiddleButtonCtrl = false;
        }

        if (this->mouseMiddleButtonAlt)
        {
            INPUT input[2] = { };

            input[0].type = INPUT_KEYBOARD;
            input[1].type = INPUT_MOUSE;

            input[0].ki.wVk = VK_CONTROL;
            input[1].mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;

            IoSymulator::keyPressedByUs = true;
            ::SendInput(ARRAYSIZE(input), input, sizeof(*input));
            IoSymulator::keyPressedByUs = false;

            ::Sleep(10);

            input[0].ki.dwFlags = KEYEVENTF_KEYUP;
            input[1].mi.dwFlags = MOUSEEVENTF_RIGHTUP;

            IoSymulator::keyPressedByUs = true;
            ::SendInput(ARRAYSIZE(input), input, sizeof(*input));
            IoSymulator::keyPressedByUs = false;

            this->PressEnter(input);

            this->mouseMiddleButtonAlt = false;
        }

        if (this->mouseWheel != 0)
        {
            INPUT input[2] = { };

            input[0].type = INPUT_MOUSE;
            input[1].type = INPUT_MOUSE;

            if (this->mouseWheel > 0)
            {
                input[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
                input[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;
                this->mouseWheel--;
            }
            else
            {
                input[0].mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
                input[1].mi.dwFlags = MOUSEEVENTF_RIGHTUP;
                this->mouseWheel++;
            }

            IoSymulator::keyPressedByUs = true;
            ::SendInput(ARRAYSIZE(input), input, sizeof(*input));
            IoSymulator::keyPressedByUs = false;

            if (this->mouseWheel != 0)
            {
                if (this->mouseWheel > 0)
                {
                    ::Sleep(10);
                }

                ::SetEvent(this->waitEvent);
            }
        }

        ::EnterCriticalSection(&this->lock);
        for (int32_t index = 0; index < RegistryData::maxData; index++)
        {
            if (this->run[index] && !this->pause)
            {
                bool click = true;

                int64_t cachedLastTime = this->timeLast[index];
                int32_t cachedTimeWait = this->timeWait[index];

                int64_t timeMightBeLast;
                int32_t timeRelativeToClick = (timeCurrent - cachedLastTime) % cachedTimeWait;
                if (timeRelativeToClick < IoSymulator::precision)
                {
                    timeMightBeLast = timeCurrent - timeRelativeToClick;
                }
                else if (cachedTimeWait - timeRelativeToClick < IoSymulator::precision)
                {
                    timeMightBeLast = timeCurrent + cachedTimeWait - timeRelativeToClick;
                }
                else
                {
                    timeMightBeLast = timeCurrent - timeRelativeToClick;
                    click = false;
                }

                uint32_t timerForNextClick = (uint32_t)(timeMightBeLast + cachedTimeWait - timeCurrent);
                if (minWait > timerForNextClick)
                {
                    minWait = timerForNextClick;
                }

                if (click)
                {
                    if (timeCurrent > cachedLastTime + cachedTimeWait - IoSymulator::precision) // Passed more then timeWait
                    {
                        this->timeLast[index] = timeMightBeLast;

                        clickedOn[index] = true;
                        clickedOn[RegistryData::maxData] = true;
                    }
                }
            }
        }
        ::LeaveCriticalSection(&this->lock);

        if (clickedOn[RegistryData::maxData])
        {
            if (minWait != INFINITE && minWait > (IoSymulator::keyDelay + (IoSymulator::keyRandom / 2)))
            {
                minWait += (rand() % IoSymulator::keyRandom) - (IoSymulator::keyDelay + (IoSymulator::keyRandom / 2));
            }

            for (int32_t index = 0; index < RegistryData::maxButtonData; index++)
            {
                if (clickedOn[index])
                {
                    INPUT input = { };

                    input.type = INPUT_KEYBOARD;

                    input.ki.wVk = 0x31 + index;
                    input.ki.dwFlags = 0;

                    IoSymulator::keyPressedByUs = true;
                    ::SendInput(1, &input, sizeof(input));
                    IoSymulator::keyPressedByUs = false;
                }
            }

            if (clickedOn[4] | clickedOn[5])
            {
                INPUT input = { };

                input.type = INPUT_MOUSE;

                if (clickedOn[4])
                {
                    input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
                }

                if (clickedOn[5])
                {
                    input.mi.dwFlags |= MOUSEEVENTF_RIGHTDOWN;
                }

                IoSymulator::keyPressedByUs = true;
                ::SendInput(1, &input, sizeof(input));
                IoSymulator::keyPressedByUs = false;
            }

            ::Sleep(IoSymulator::keyDelay);

            for (int32_t index = 0; index < RegistryData::maxButtonData; index++)
            {
                if (clickedOn[index])
                {
                    INPUT input = { };

                    input.type = INPUT_KEYBOARD;

                    input.ki.wVk = 0x31 + index;
                    input.ki.dwFlags = KEYEVENTF_KEYUP;

                    ::SendInput(1, &input, sizeof(input));
                }
            }

            if (clickedOn[4] | clickedOn[5])
            {
                INPUT input = { };

                input.type = INPUT_MOUSE;

                if (clickedOn[4])
                {
                    input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
                }

                if (clickedOn[5])
                {
                    input.mi.dwFlags |= MOUSEEVENTF_RIGHTUP;
                }

                ::SendInput(1, &input, sizeof(input));
            }
        }
        else
        {
            if (minWait != INFINITE && minWait > IoSymulator::keyRandom / 2)
            {
                minWait += (rand() % IoSymulator::keyRandom) - (IoSymulator::keyRandom / 2);
            }
        }
    }

    return 0;
}

LRESULT CALLBACK IoSymulator::KeyboardEvent(int32_t code, WPARAM wParam, LPARAM lParam)
{
    if (code != HC_ACTION || IoSymulator::keyPressedByUs)
    {
        return ::CallNextHookEx(nullptr, code, wParam, lParam);
    }

    return IoSymulator::ioSymulator->Keyboard(code, wParam, lParam);
}

LRESULT CALLBACK IoSymulator::MouseEvent(int32_t code, WPARAM wParam, LPARAM lParam)
{
    if (code != HC_ACTION || IoSymulator::keyPressedByUs)
    {
        return ::CallNextHookEx(nullptr, code, wParam, lParam);
    }

    return IoSymulator::ioSymulator->Mouse(code, wParam, lParam);
}

DWORD __stdcall IoSymulator::ThreadCallback(IoSymulator* symulator)
{
    return IoSymulator::ioSymulator->Thread();
}
