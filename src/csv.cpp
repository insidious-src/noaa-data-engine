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

#include <noaa/csv.h>
#include <fstream>
#include <sstream>
#include <algorithm>

bool CSVParser::append(string_type const& strFilePath)
{
    std::fstream file (strFilePath);
    string_type  line;

    if(!file.is_open ()) return false;

    // optimize memory allocation
    auto lineCount = std::count (std::istreambuf_iterator<char> (file),
                                 std::istreambuf_iterator<char> (), '\n');

    m_data.reserve (static_cast<size_type> (lineCount) + 1);
    file.seekg(0);

    // read each line
    while (!file.eof ())
    {
        std::getline (file, line);

        if (line == "") continue;

        std::vector<string_type> record;
        std::istringstream       stream (line);

        // optimize memory allocation
        auto fieldCount = std::count (std::istreambuf_iterator<char> (stream),
                                      std::istreambuf_iterator<char> (), ',');

        record.reserve (static_cast<size_type> (fieldCount) + 1);
        stream.seekg(0);


        string_type field;

        // read each field
        while (stream)
        {
            if (!getline (stream, field, ',')) break;
            record.push_back (field);
        }

        m_data.push_back (record);
    }

    // sort by time
    std::sort(m_data.begin(), m_data.end(), [](fields_type const& i, fields_type const& j)
    { return (i[1] < j[1]); });

    return true;
}
