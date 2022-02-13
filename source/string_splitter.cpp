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

#include <libserial/string_splitter.h>
#include <string.h>

namespace libserial {


string_splitter::string_splitter(const std::string &str, char delimiter)
{
    _string = new char[str.length() + 1];
    memset(_string, 0, str.length() + 1);
    memcpy(_string, str.data(), str.length());

    char *head = _string;
    char *tail = head;

    bool done = false;

    while (!done) {

        while ((*tail != delimiter) && (*tail != 0)) {
            tail++;
        }

        done = (*tail == 0);

        *tail = 0;
        _index.push_back(head);

        tail++;
        head = tail;
    }
}

string_splitter::~string_splitter()
{
    delete [] _string;
}

size_t string_splitter::count() const
{
    return _index.size();
}

const char * string_splitter::operator [](size_t index) const
{
    if (index <= _index.size()) {
        return _index[index];

    } else {
        return nullptr;
    }
}

}
