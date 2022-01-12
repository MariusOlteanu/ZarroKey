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

#include <Windows.h>
#include <stdint.h>

class RegistryData
{
public:

    inline static constexpr uint32_t maxData = 6;
    inline static constexpr uint32_t maxButtonData = 4;
    inline static constexpr uint32_t minTimeValue = 300;
    inline static constexpr uint32_t maxTimeValue = 99999;
    inline static constexpr uint32_t timeValueMaxChars = 5;
    inline static constexpr uint32_t profilesCount = 7;
    inline static constexpr uint32_t profilesNameLength = 29;

    ~RegistryData();

    void Init(const wchar_t* path);

    inline uint32_t GetCurrentSelection() { return this->data.selected; };
    void SetCurrentSelection(uint32_t selected);

    template <size_t DataSize = RegistryData::profilesNameLength + 1>
    void GetProfileName(wchar_t (&profileName)[DataSize]);

    wchar_t* GetProfileName(uint32_t position);

    template <size_t DataSize = RegistryData::profilesNameLength + 1>
    bool ChangeProfileName(wchar_t (&profileName)[DataSize]);

    bool Apply();
    void Close();

    bool checkbox[RegistryData::maxData] = { false, false, false, false, false, false };

    int32_t timeInt[RegistryData::maxData] = { 800, 800, 800, 800, 800, 800 };

    wchar_t timeValue[RegistryData::maxData][timeValueMaxChars + 1];

protected:

    inline static constexpr uint32_t version = 0x11776403;

    typedef struct _registry_data_t
    {
        bool checkbox[maxData];
        int32_t timeInt[maxData];
        wchar_t profileName[profilesNameLength + 1];
    } registry_data_t;

    struct stored_data_t
    {
        uint32_t version;
        uint32_t selected;
        registry_data_t data[profilesCount];
    };

    stored_data_t data;

    HKEY key = nullptr;

    void UpdateDefaultData();
};
