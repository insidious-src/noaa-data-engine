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

#ifndef JSON_H
#define JSON_H

#include <QtCore/QJsonDocument>

class CSVParser;

class JsonParser
{
public:
    typedef QString string_type;

    JsonParser(CSVParser const& csv, string_type const& file);

private:
    QJsonDocument    m_gJsonDoc   ;
    string_type      m_strFilePath;
    CSVParser const* m_pCsvParser ;
};

#endif // JSON_H
