/*
 * Copyright 2018 K. Petrov <fymfifa@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef CSV_H
#define CSV_H

#include <string>
#include <vector>

class CSVParser
{
public:
    typedef std::string                           string_type;
    typedef std::vector<std::vector<string_type>> vector_type;
    typedef std::size_t                           size_type  ;

    bool append (string_type const& file_path);

    CSVParser& operator += (string_type const& file_path)
    { append(file_path); return *this; }

    size_type lineCount () const noexcept
    { return m_data.size(); }

    size_type fieldCount () const noexcept
    { return m_data.empty() ? size_type () : m_data[0].size(); }

    template <std::size_t N>
    string_type get (size_type idx) const
    { return m_data[idx][N]; }

private:
    vector_type m_data;
};

#endif // CSV_H
