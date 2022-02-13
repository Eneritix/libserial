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

#include <libserial/serial_port.h>
#include "serial_port_impl.h"

#if defined(WIN32)
#include "serial_port_impl_win32.h"
#else
#include "serial_port_impl_linux.h"
#endif


namespace libserial {

serial_port::serial_port() :
    _impl(nullptr)
{

}

serial_port::serial_port(serial_port_impl *impl) :
    _impl(impl)
{

}

serial_port::serial_port(const std::string &error_string) :
    _impl(nullptr),
    _error_string(error_string)
{

}

serial_port::operator bool() const
{
    if (_impl) {
        return _impl->is_valid();
    } else {
        return false;
    }
}

std::string serial_port::error_string() const
{
    if (_impl) {
        return _impl->error_string();
    } else {
        return _error_string;
    }
}

size_t serial_port::read(uint8_t *buffer, size_t length, uint32_t timeout_ms)
{
    size_t result = 0;

    if (_impl) {
        result = _impl->read(buffer, length, timeout_ms);
    }

    return result;
}

size_t serial_port::write(const uint8_t *buffer, size_t length)
{
    size_t result = 0;

    if (_impl) {
        result = _impl->write(buffer, length);
    }

    return result;
}

}
