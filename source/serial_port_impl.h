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

#ifndef SERIAL_PORT_IMPL_H
#define SERIAL_PORT_IMPL_H

#include <stdint.h>
#include <stddef.h>
#include <string>

namespace libserial {

class serial_port_impl
{
public:
    virtual ~serial_port_impl() {}
    virtual bool is_valid() const = 0;
    virtual std::string error_string() const = 0;
    virtual size_t read(uint8_t *buffer, size_t length, uint32_t timeout_ms) = 0;
    virtual size_t write(const uint8_t *buffer, size_t length) = 0;
};

}

#endif
