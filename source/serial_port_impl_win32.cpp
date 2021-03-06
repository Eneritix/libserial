/*
    Copyright (c) 2022 Eneritix (Pty) Ltd
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:
    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#if defined(WIN32)
#include "serial_port_impl_win32.h"
#include <libserial/serial_port.h>


namespace libserial {

static std::string GetLastErrorAsString()
{
    DWORD errorMessageID = ::GetLastError();
    if(errorMessageID == 0) {
        return std::string();
    }

    LPSTR messageBuffer = nullptr;
    size_t size = FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        errorMessageID,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&messageBuffer,
        0,
        NULL);

    std::string message(messageBuffer, size);

    LocalFree(messageBuffer);

    return message;
}

serial_port serial_port::open(
    const std::string &device,
    unsigned int baud_rate,
    unsigned int data_bits,
    serial_port::parity parity,
    serial_port::stop_bits stop_bits)
{
    std::string devicePath(std::string("\\\\.\\") + device);

    HANDLE handle = CreateFileA(
        devicePath.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);

    if (handle == INVALID_HANDLE_VALUE) {
        return serial_port(std::string("Could not open serial port: ") + GetLastErrorAsString());
    }

    DCB dcb;
    memset(&dcb, 0, sizeof(dcb));
    dcb.DCBlength = sizeof(dcb);

    if (GetCommState(handle, &dcb) == FALSE) {
        return serial_port(std::string("Could not retrieve comm state: ") + GetLastErrorAsString());
    }

    dcb.BaudRate = CBR_115200;
    dcb.ByteSize = 8;
    dcb.StopBits = ONESTOPBIT;
    dcb.Parity = NOPARITY;
    if (SetCommState(handle, &dcb) == FALSE) {
        return serial_port(std::string("Could not configure serial port: ") + GetLastErrorAsString());
    }


    COMMTIMEOUTS timeouts;
    memset(&timeouts, 0, sizeof(timeouts));

    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 100;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = MAXDWORD;
    timeouts.WriteTotalTimeoutMultiplier = MAXDWORD;
    if (SetCommTimeouts(handle, &timeouts) == FALSE) {
        return serial_port(std::string("Could not set serial port timeouts: ") + GetLastErrorAsString());
    }

    return serial_port(new serial_port_impl_win32(handle));
}

serial_port_impl_win32::serial_port_impl_win32(HANDLE handle) :
    _handle(handle) {

}

bool serial_port_impl_win32::is_valid() const
{
    return (_handle != INVALID_HANDLE_VALUE);
}

std::string serial_port_impl_win32::error_string() const
{
    return _error_string;
}

size_t serial_port_impl_win32::read(uint8_t *buffer, size_t length, uint32_t timeout_ms)
{
    DWORD bytesRead = 0;
    BOOL result = false;
    if (_handle != INVALID_HANDLE_VALUE) {
        result = ReadFile(_handle, buffer, length, &bytesRead, NULL);
    }

    if (!result) {
        CloseHandle(_handle);
        _handle = INVALID_HANDLE_VALUE;
        bytesRead = 0;
    }

    return bytesRead;
}

size_t serial_port_impl_win32::write(const uint8_t *buffer, size_t length)
{
    DWORD bytesWritten = 0;
    BOOL result = false;
    if (_handle != INVALID_HANDLE_VALUE) {
        result = WriteFile(_handle, buffer, length, &bytesWritten, NULL);
    }

    if (!result) {
        CloseHandle(_handle);
        _handle = INVALID_HANDLE_VALUE;
        bytesWritten = 0;
    }

    return bytesWritten;
}

}

#endif
