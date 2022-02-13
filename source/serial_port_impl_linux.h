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

#ifndef SERIAL_PORT_IMPL_LINUX_H
#define SERIAL_PORT_IMPL_LINUX_H

#include "serial_port_impl.h"
#include <libserial/serial_port.h>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/poll.h>

namespace libserial {


class serial_port_impl_linux : public serial_port_impl
{
public:
    serial_port_impl_linux(int fd) :
        _fd(fd) {

    }

    bool is_valid() const {
        return (_fd != -1);
    }

    size_t read(uint8_t *buffer, size_t length, uint32_t timeout_ms)
    {
        ssize_t bytes_read = 0;

        struct pollfd pfd[1];
        pfd[0].fd = _fd;
        pfd[0].events = POLLIN;
        pfd[0].revents = 0;

        if (poll(pfd, 1, timeout_ms) > 0) {
            if (pfd[0].revents & POLLIN) {
                bytes_read = ::read(_fd, buffer, length);
            }
        }

        if (bytes_read < 0) {
            ::close(_fd);
            _fd = -1;
            return -1;
        }

        return bytes_read;
    }

    size_t write(const uint8_t *buffer, size_t length)
    {
        while (length) {
            ssize_t bytes_written = ::write(_fd, buffer, length);
            if (bytes_written < 0) {
                ::close(_fd);
                _fd = -1;
                return -1;
            }

            buffer += bytes_written;
            length -= bytes_written;
        }

        return 0;
    }

private:
    int _fd;
};

}

#endif
