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

#ifndef SERIAL_PORT_H
#define SERIAL_PORT_H

#include <stdint.h>
#include <stddef.h>
#include <string>
#include <memory>

namespace libserial {

class serial_port_impl;

class serial_port
{
public:
    serial_port();
    operator bool() const;
    std::string error_string() const;
    size_t read(uint8_t *buffer, size_t length, uint32_t timeout_ms);
    size_t write(const uint8_t *buffer, size_t length);

public:
    static serial_port open(const std::string &device);

private:
    serial_port(serial_port_impl *impl);
    serial_port(const std::string &error_string);

private:
    std::shared_ptr<serial_port_impl> _impl;
    std::string _error_string;
};

}

#endif
