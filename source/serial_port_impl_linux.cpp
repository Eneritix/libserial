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

#if !defined(WIN32)
#include "serial_port_impl_linux.h"
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
#include <map>

namespace libserial {

static const std::map<unsigned int, speed_t> baud_rate_map =
{
    { 50            ,B50            },
    { 75            ,B75            },
    { 110           ,B110           },
    { 134           ,B134           },
    { 150           ,B150           },
    { 200           ,B200           },
    { 300           ,B300           },
    { 600           ,B600           },
    { 1200          ,B1200          },
    { 1800          ,B1800          },
    { 2400          ,B2400          },
    { 4800          ,B4800          },
    { 9600          ,B9600          },
    { 19200         ,B19200         },
    { 38400         ,B38400         },
    { 57600         ,B57600         },
    { 115200        ,B115200        },
    { 230400        ,B230400        },
    { 460800        ,B460800        },
    { 500000        ,B500000        },
    { 576000        ,B576000        },
    { 921600        ,B921600        },
    { 1000000       ,B1000000       },
    { 1152000       ,B1152000       },
    { 1500000       ,B1500000       },
    { 2000000       ,B2000000       },
    { 2500000       ,B2500000       },
    { 3000000       ,B3000000       },
    { 3500000       ,B3500000       },
    { 4000000       ,B4000000       },
};


static int _map_baud_rate(unsigned int baud_rate)
{
    speed_t result = B0;
    const auto i = baud_rate_map.find(baud_rate);
    if (i != baud_rate_map.end()) {
        result = i->second;
    }

    return result;
}

static int _map_data_bits(unsigned int data_bits)
{
    switch (data_bits) {
    case 5: return CS5;
    case 6: return CS6;
    case 7: return CS7;
    case 8: return CS8;
    default: return CS8;
    }
}

serial_port serial_port::open(
    const std::string &device,
    unsigned int baud_rate,
    unsigned int data_bits,
    serial_port::parity parity,
    serial_port::stop_bits stop_bits)
{
    int fd;

    fd = ::open(device.c_str(), O_RDWR | O_NOCTTY);
    if (fd == -1) {
        return serial_port(std::string("Could not open serial port: ") + std::string(strerror(errno)));
    }

    struct termios options;

    // Set port options
    tcgetattr(fd, &options);

    cfsetispeed(&options, _map_baud_rate(baud_rate));
    cfsetospeed(&options, _map_baud_rate(baud_rate));

    options.c_cflag |= (CLOCAL | CREAD);


    // Data bits
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= _map_data_bits(data_bits);

    // Parity
    options.c_cflag &= ~PARENB;

    if (parity == serial_port::parity::odd) {
        options.c_cflag |= PARENB;
        options.c_cflag |= PARODD;

    } else if (parity == serial_port::parity::even) {
        options.c_cflag |= PARENB;
        options.c_cflag &= ~PARODD;
    }

    // Stop bits
    if (stop_bits == serial_port::stop_bits::one) {
        options.c_cflag &= ~CSTOPB;

    } else {
        options.c_cflag |= CSTOPB;
    }


    options.c_cc[VMIN] = 1;
    options.c_cc[VTIME] = 5;

    // Manual RTS
    //options.c_cflag |= CRTSCTS;
    //options.c_cflag &= ~CNEW_RTSCTS;

    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_iflag &= ~(IXON | IXOFF | IXANY);
    options.c_oflag &= ~(OPOST | ONLCR);

    cfmakeraw(&options);
    tcsetattr(fd, TCSANOW, &options);

    // Flush
    tcflush(fd, TCIOFLUSH);

    return serial_port(new serial_port_impl_linux(fd));
}

serial_port_impl_linux::serial_port_impl_linux(int fd) :
    _fd(fd)
{

}

serial_port_impl_linux::~serial_port_impl_linux()
{
    if (_fd != -1) {
        ::close(_fd);
    }
}

bool serial_port_impl_linux::is_valid() const
{
    return (_fd != -1);
}

std::string serial_port_impl_linux::error_string() const
{
    return _error_string;
}

size_t serial_port_impl_linux::read(uint8_t *buffer, size_t length, uint32_t timeout_ms)
{
    ssize_t bytes_read = 0;
    int result;

    if (_fd == -1) {
        return 0;
    }

    struct pollfd pfd[1];
    pfd[0].fd = _fd;
    pfd[0].events = POLLIN;
    pfd[0].revents = 0;

    result = poll(pfd, 1, timeout_ms);

    // If poll() fails we assume that the port is in a bad enough state to not continue
    if (result < 0) {
        _error_string = strerror(errno);
        ::close(_fd);
        _fd = -1;
        return 0;
    }

    if (result > 0) {
        if (pfd[0].revents & POLLIN) {
            bytes_read = ::read(_fd, buffer, length);
        }
    }

    if (bytes_read < 0) {
        _error_string = strerror(errno);
        ::close(_fd);
        _fd = -1;
        return 0;
    }

    return bytes_read;
}

size_t serial_port_impl_linux::write(const uint8_t *buffer, size_t length)
{
    if (_fd == -1) {
        return 0;
    }

    while (length) {
        ssize_t bytes_written = ::write(_fd, buffer, length);
        if (bytes_written < 0) {
            _error_string = strerror(errno);
            ::close(_fd);
            _fd = -1;
            return 0;
        }

        buffer += bytes_written;
        length -= bytes_written;
    }

    return 0;
}

}

#endif
