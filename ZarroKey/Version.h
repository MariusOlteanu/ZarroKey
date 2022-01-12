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

#define PRODUCT_VERSION_MAJOR           2
#define PRODUCT_VERSION_MINOR           0
#define PRODUCT_VERSION_MAINTENANCE     0
#define PRODUCT_VERSION_BUILD           002
#define REVISION_NUMBER                 0

#define FILE_VERSION_MAJOR              PRODUCT_VERSION_MAJOR
#define FILE_VERSION_MINOR              PRODUCT_VERSION_MINOR
#define FILE_VERSION_MAINTENANCE        PRODUCT_VERSION_MAINTENANCE

#define STRINGIFYVER(major, minor, maintenance, build) #major "." #minor "." #maintenance "." #build
#define TOSTRINGVER(major, minor, maintenance, build) STRINGIFYVER(major, minor, maintenance, build)

#define FILE_VERSION            PRODUCT_VERSION_MAJOR, PRODUCT_VERSION_MINOR, PRODUCT_VERSION_MAINTENANCE, PRODUCT_VERSION_BUILD
#define PRODUCT_VERSION         FILE_VERSION_MAJOR, FILE_VERSION_MINOR, FILE_VERSION_MAINTENANCE, REVISION_NUMBER

#define FILE_VERSION_STRING     TOSTRINGVER(PRODUCT_VERSION_MAJOR, PRODUCT_VERSION_MINOR, PRODUCT_VERSION_MAINTENANCE, PRODUCT_VERSION_BUILD)
#define PRODUCT_VERSION_STRING  TOSTRINGVER(FILE_VERSION_MAJOR, FILE_VERSION_MINOR, FILE_VERSION_MAINTENANCE, REVISION_NUMBER)

inline constexpr unsigned int Version = (PRODUCT_VERSION_MAJOR << 0x18) | (PRODUCT_VERSION_MINOR << 0x10) | (PRODUCT_VERSION_MAINTENANCE << 0x08) | (PRODUCT_VERSION_BUILD << 0x00);
