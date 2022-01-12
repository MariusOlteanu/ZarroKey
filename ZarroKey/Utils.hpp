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

#include <iostream>
#include <string>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

template <typename Type = HANDLE, typename CleanupFunction = decltype(&::CloseHandle), Type Null = nullptr>
class windows_ptr
{
public:

    constexpr windows_ptr(Type handle = Null) noexcept : handle(handle), Cleanup(::CloseHandle) {};
    explicit constexpr windows_ptr(CleanupFunction Cleanup, Type handle = Null) noexcept : handle(handle), Cleanup(Cleanup) {};

    constexpr ~windows_ptr() noexcept
    {
        if (this->handle != Null)
        {
            Cleanup(handle);
        }
    }

    constexpr windows_ptr(const windows_ptr& other) noexcept : handle(other.handle) {};
    constexpr windows_ptr& operator=(const windows_ptr& other) noexcept { this->handle = other.handle; return *this; };
    constexpr windows_ptr(windows_ptr&& other) noexcept : handle(std::move(other.handle)) {};
    constexpr windows_ptr& operator=(windows_ptr&& other) noexcept { this->handle = std::move(other.handle); return *this; };

    constexpr windows_ptr& operator=(Type newHandle)
    {
        if (newHandle == Null)
        {
            throw std::runtime_error("Failed to create the windows object: " + std::to_string(::GetLastError()));
        }

        this->handle = newHandle;

        return *this;
    }

    constexpr explicit operator bool() const noexcept { return this->handle != Null; }
    constexpr bool operator!() const noexcept { return this->handle == Null; }
    constexpr bool operator==(Type newHandle) { return this->handle == newHandle; }

    constexpr operator Type() const noexcept { return this->handle; }

protected:

    Type handle;

    CleanupFunction Cleanup;
};
