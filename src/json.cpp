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

#include <noaa/json.h>
#include <noaa/csv.h>
#include <vector>
#include <iostream>
#include <QtCore/QFile>
#include <QtCore/QTimeZone>
#include <QtCore/QDateTime>
#include <QtCore/QJsonValue>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonDocument>

class TimeZone
{
public:
    typedef QString string_type;

    TimeZone(string_type&& date_time, int time_zone = 2)
    : m_gDateTime (QDateTime::fromString(date_time.replace("\"", ""), "yyyy-MM-dd HH:mm:ss")),
      m_nTimeZone (time_zone)
    { }

    string_type toStringUTC () const
    {
        return string_type::number(m_gDateTime.time().toString("HH").toInt() + m_nTimeZone) +
               ":" + m_gDateTime.time().toString("mm");
    }

    string_type toString () const
    { return m_gDateTime.toString("yyyy-MM-dd HH:mm:ss"); }

private:
    QDateTime m_gDateTime;
    int       m_nTimeZone;
};

// ==========================================================

bool JsonParser::save ()
{
    QFile file (m_strFilePath);

    if (file.exists()) file.remove();

    file.open(QFile::Append);

    if (!m_pCsv->lineCount() or !m_pCsv->fieldCount() or !file.isOpen()) return false;

    //QJsonObject                fileSection     ;
    QJsonObject                jsonObjects     ;
    std::vector<QJsonDocument> jsonSections (2);

    //fileSection.insert("title"    , QJsonValue("DSWRF"));
    //fileSection.insert("date"     , QJsonValue(TimeZone(m_pCsv->get<0>(0)).toString()));
    //fileSection.insert("longetude", QJsonValue(m_pCsv->get<4>(0).toDouble()));
    //fileSection.insert("latitude" , QJsonValue(m_pCsv->get<5>(0).toDouble()));

    //jsonSections.push_back(QJsonDocument(fileSection));

    for (auto i = 0U; i < m_pCsv->lineCount(); ++i)
    {
        jsonObjects.insert(TimeZone(m_pCsv->get<1>(i)).toStringUTC(), QJsonValue(m_pCsv->get<6>(i).toInt()));
    }

    jsonSections.push_back(QJsonDocument(jsonObjects));

    for (auto i = 0U; i < jsonSections.size(); ++i)
    {
        file.write(jsonSections[i].toJson());
    }

    return true;
}
