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

#include "RegistryData.hpp"

#include <Windows.h>

static const wchar_t* DATA_KEY = L"DefaultList";
static const wchar_t* DATA_SUB_KEY = L"DefaultData";
static const wchar_t DEFAULT_PROFILES_NAME[RegistryData::profilesCount][RegistryData::profilesNameLength + 1] =
{
    L"Vizila",
    L"Poca",
    L"Taranul",
    L"Rocherul",
    L"Pescarul",
    L"Aumadoare",
    L"Crestinul",
};

RegistryData::~RegistryData()
{
    this->Close();
}

void RegistryData::Init(const wchar_t* path)
{
    LSTATUS status;

    bool dirty = false;

    DWORD disposition;
    DWORD dataType;
    DWORD dataSize = sizeof(this->data);

    status = ::RegCreateKeyExW(HKEY_CURRENT_USER, path, 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, nullptr, &key, &disposition);
    if (status != ERROR_SUCCESS)
    {
        goto error;
    }

    dataType = REG_BINARY;
    status = ::RegGetValueW(key, DATA_KEY, DATA_SUB_KEY, RRF_RT_REG_BINARY, &dataType, &this->data, &dataSize);
    if (status != ERROR_SUCCESS || dataSize != sizeof(this->data) || dataType != REG_BINARY || this->data.version != RegistryData::version)
    {
        this->UpdateDefaultData();
        status = ::RegSetKeyValueW(key, DATA_KEY, DATA_SUB_KEY, REG_BINARY, &data, dataSize);
    }

    for (uint32_t index = 0; index < RegistryData::profilesCount; index++)
    {
        data.data[this->data.selected].profileName[RegistryData::profilesNameLength] = L'\0';

        for (uint32_t jndex = 0; jndex < RegistryData::maxData; jndex++)
        {
            if (data.data[index].timeInt[jndex] < RegistryData::minTimeValue || data.data[index].timeInt[jndex] > RegistryData::maxTimeValue)
            {
                dirty = true;

                index = RegistryData::profilesCount;

                break;
            }
        }
    }

    if (dirty)
    {
        this->UpdateDefaultData();
        status = ::RegSetKeyValueW(key, DATA_KEY, DATA_SUB_KEY, REG_BINARY, &data, dataSize);
    }

    for (uint32_t index = 0; index < RegistryData::maxData; index++)
    {
        this->checkbox[index] = data.data[this->data.selected].checkbox[index];
        this->timeInt[index] = data.data[this->data.selected].timeInt[index];
        ::_itow_s(this->timeInt[index], this->timeValue[index], 10);
    }

    return;

error:

    this->UpdateDefaultData();
}

void RegistryData::Close()
{
    if (this->key != nullptr)
    {
        ::RegCloseKey(this->key);
        this->key = nullptr;
    }
}

void RegistryData::SetCurrentSelection(uint32_t selected)
{
    if (selected >= RegistryData::profilesCount)
    {
        selected = RegistryData::profilesCount - 1;
    }

    this->data.selected = selected;

    for (uint32_t index = 0; index < RegistryData::maxData; index++)
    {
        this->checkbox[index] = data.data[selected].checkbox[index];
        this->timeInt[index] = data.data[selected].timeInt[index];
        ::_itow_s(this->timeInt[index], this->timeValue[index], 10);
    }
};

template
void RegistryData::GetProfileName<RegistryData::profilesNameLength + 1>(wchar_t (&profileName)[RegistryData::profilesNameLength + 1]);

template <size_t DataSize>
void RegistryData::GetProfileName(wchar_t (&profileName)[DataSize])
{
    ::memcpy(profileName, data.data[this->data.selected].profileName, DataSize * 2);
}

wchar_t* RegistryData::GetProfileName(uint32_t position)
{
    return this->data.data[position].profileName;
}

template
bool RegistryData::ChangeProfileName<RegistryData::profilesNameLength + 1>(wchar_t (&profileName)[RegistryData::profilesNameLength + 1]);

template <size_t DataSize>
bool RegistryData::ChangeProfileName(wchar_t (&profileName)[DataSize])
{
    LSTATUS status;

    ::memcpy(data.data[this->data.selected].profileName, profileName, DataSize * 2);

    status = ::RegSetKeyValueW(key, DATA_KEY, DATA_SUB_KEY, REG_BINARY, &data, sizeof(this->data));
    if (status != ERROR_SUCCESS)
    {
        ::MessageBoxW(nullptr, L"Failed to save the data", L"Berila!", MB_OK);
    }

    return status == ERROR_SUCCESS;
}

bool RegistryData::Apply()
{
    LSTATUS status;

    for (uint32_t index = 0; index < RegistryData::maxData; index++)
    {
        this->data.data[this->data.selected].timeInt[index] = this->timeInt[index];
        this->data.data[this->data.selected].checkbox[index] = this->checkbox[index];
    }

    status = ::RegSetKeyValueW(key, DATA_KEY, DATA_SUB_KEY, REG_BINARY, &data, sizeof(this->data));
    if (status != ERROR_SUCCESS)
    {
        ::MessageBoxW(nullptr, L"Failed to save the data", L"Berila!", MB_OK);
    }

    return status == ERROR_SUCCESS;
}

void RegistryData::UpdateDefaultData()
{
    ZeroMemory(&this->data, sizeof(this->data));

    this->data.version = RegistryData::version;
    this->data.selected = 0;

    for (uint32_t index = 0; index < RegistryData::maxData; index++)
    {
        ::_itow_s(this->timeInt[index], this->timeValue[index], 10);
    }

    for (uint32_t index = 0; index < RegistryData::profilesCount; index++)
    {
        ::memcpy(data.data[index].profileName, DEFAULT_PROFILES_NAME[index], (profilesNameLength + 1) * sizeof(wchar_t));

        for (uint32_t jndex = 0; jndex < RegistryData::maxData; jndex++)
        {
            data.data[index].timeInt[jndex] = this->timeInt[jndex];
        }
    }
}
