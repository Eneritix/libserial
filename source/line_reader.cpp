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

#include <libserial/line_reader.h>


namespace libserial {

line_reader::line_reader(const serial_port &serial_port, size_t read_cache_size) :
    _serial_port(serial_port),
    _read_cache_index(0),
    _read_cache_count(0),
    _cr(false)
{
    _read_cache.resize(read_cache_size);

}

std::string line_reader::read_line(uint32_t timeout_ms)
{
    std::string result;

    if (_read_cache_count == 0) {
        _read_cache_count = _serial_port.read(_read_cache.data(), _read_cache.size(), timeout_ms);
        _read_cache_index = 0;
    }

    bool end_of_line = false;

    while (_read_cache_count && !end_of_line) {
        if (_cr) {
            if (_read_cache[_read_cache_index] == '\n') {
                end_of_line = true;
            } else {
                _current_line += 'r';
                _current_line += _read_cache[_read_cache_index];
            }

            if (_read_cache[_read_cache_index] != '\r') {
                _cr = false;
            }

        } else {
            if (_read_cache[_read_cache_index] == '\r') {
                _cr = true;

            } else if (_read_cache[_read_cache_index] == '\n') {
                end_of_line = true;

            } else {
                _current_line += _read_cache[_read_cache_index];
            }
        }

        _read_cache_count--;
        _read_cache_index++;
    }

    if (end_of_line) {
        result = _current_line;
        _current_line = std::string();
    }

    return result;
}

}
