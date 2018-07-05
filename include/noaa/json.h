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

#include <QtCore/QString>
#include <QtCore/QJsonDocument>
#include <functional>

class CSVParser;

class RedNodeJson
{
public:
    typedef QString                          string_type;
    typedef std::size_t                      size_type  ;
    typedef std::function<double(size_type)> func_type  ;

    RedNodeJson () = delete;
    QJsonDocument parse();
    bool save(QJsonDocument const& json, string_type const& file_path);
    bool save(string_type const& file_path);

    template <typename Processor>
    RedNodeJson(CSVParser& csv, Processor fn)
    : m_fn  (fn  ),
      m_pCsv(&csv)
    { }

    CSVParser& csv() const noexcept
    { return *m_pCsv; }

private:
    func_type  m_fn  ;
    CSVParser* m_pCsv;
};

#endif // JSON_H
