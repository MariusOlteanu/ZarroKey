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

#include "HelperText.hpp"
#include "Version.h"

const wchar_t* AppTitle = L"Berila!";

const wchar_t* InfoText = L""
    "Info:\n"
    "\n"
    "  Ctrl + 1/2/3/4\t- Activate skills from keys 1/2/3/4\n"
    "  Ctrl + 5/6\t- Activate skills from Left/Right click\n"
    "  Alt  + 1/2/3/4\t- Deactivate skills from keys 1/2/3/4\n"
    "  Alt  + 5/6\t- Deactivate skills from Left/Right click\n"
    "  Ctrl + `/0\t- Activate skills set through checkboxes\n"
    "  Alt  + `/0\t- Deactivate skills set through checkboxes\n"
    "  `/0\t\t- Pause/Resume skills\n"
    "\n"
    "  F2\t\t- Destroy an item to the forge\n"
    "  F3\t\t- Buy 50 items from Kadala\n"
    "  F4/Ctrl + F2\t- Repair and delete all non legendary items\n"
    "  F5\t\t- 50 Left clicks\n"
    "  F6\t\t- 10 Left clicks\n"
    "\n"
    "Remarks:\n"
    "\n"
    "  Time in the checkboxes is in milliseconds\n"
    "\n"
    "About:\n"
    "\n"
    "  If you have any complaints please keep them to yourself!\n"
    "\n"
    "  Made by Zarro\t\t\t\t(C)opyright by Zarro\n"
    "  email: olteanu.marius@gmail.com\t\tver. " FILE_VERSION_STRING;

const wchar_t* AboutText = L""
    "About:\n"
    "\n"
    "  If you have any complaints please keep them to yourself!\n"
    "  Any resemblance to a game is purely coincidental!\n"
    "\n"
    "  Made by Zarro\t\t\t\t(C)opyright by Zarro\n"
    "  email: olteanu.marius@gmail.com\t\tver. " FILE_VERSION_STRING;
