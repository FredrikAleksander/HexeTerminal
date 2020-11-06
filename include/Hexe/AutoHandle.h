// The MIT License (MIT)

// Copyright (c) 2020 Fredrik A. Kristiansen

//  Permission is hereby granted, free of charge, to any person obtaining a
//  copy of this software and associated documentation files (the "Software"),
//  to deal in the Software without restriction, including without limitation
//  the rights to use, copy, modify, merge, publish, distribute, sublicense,
//  and/or sell copies of the Software, and to permit persons to whom the
//  Software is furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
//  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
//  DEALINGS IN THE SOFTWARE.
#pragma once

#ifdef WIN32
#include <windows.h>
#endif

namespace Hexe
{
    // This class is designed to automatically free a resource handle when destructed
    class AutoHandle final
    {
    public:
#ifdef WIN32
        // On Windows we deal with the Windows API, which means HANDLE is used as the handle to files and pipes
        using type = HANDLE;
        static constexpr type invalid_value() { return INVALID_HANDLE_VALUE; }
#else
        // On non-Windows OS'es we deal with file descriptors
        using type = int;
        static constexpr type invalid_value() { return -1; }
#endif
    private:
        mutable type m_hHandle;

    public:
        explicit AutoHandle(type handle);
        AutoHandle();
        AutoHandle(AutoHandle &&other);
        ~AutoHandle();

        inline AutoHandle &operator=(AutoHandle &&other)
        {
            if (m_hHandle == other.m_hHandle)
                return *this;
            Release();
            m_hHandle = other.m_hHandle;
            other.m_hHandle = invalid_value();
            return *this;
        }

        AutoHandle(const AutoHandle &) = delete;
        AutoHandle &operator=(const AutoHandle &) = delete;

        inline explicit operator type() const noexcept { return m_hHandle; }
        inline operator bool() const noexcept { return m_hHandle != invalid_value(); }

        inline type *Get() { return &m_hHandle; }
        inline const type *Get() const { return &m_hHandle; }

        void Release() const;
    };
} // namespace Hexe
