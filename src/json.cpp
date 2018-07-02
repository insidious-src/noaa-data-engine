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

    int hour () const
    { return m_gDateTime.time().toString("HH").toInt() + m_nTimeZone; }

    string_type toString () const
    { return m_gDateTime.toString("yyyy-MM-dd HH:mm:ss"); }

private:
    QDateTime m_gDateTime;
    int       m_nTimeZone;
};

// ==========================================================

bool RedNodeJson::save ()
{
    typedef QPair<string_type, QJsonValue> pair_type;

    QFile file (m_strFilePath);

    if (file.exists()) file.remove();

    file.open(QFile::Append);

    if (!m_pCsv->lineCount() or !m_pCsv->fieldCount() or !file.isOpen()) return false;

    QJsonObject   jsonObject   ;
    QJsonDocument jsonDocument ;
    QJsonArray    jsonDataArray;

    jsonObject.insert("series"   , QJsonValue(QJsonArray({ m_pCsv->get<2>(0) })));
    jsonObject.insert("date"     , QJsonValue(TimeZone(m_pCsv->get<0>(0)).toString()));
    jsonObject.insert("longitude", QJsonValue(m_pCsv->get<4>(0).toDouble()));
    jsonObject.insert("latitude" , QJsonValue(m_pCsv->get<5>(0).toDouble()));

    for (auto i = 0U; i < m_pCsv->lineCount(); ++i)
    {
        jsonDataArray.append(QJsonValue(QJsonObject({ pair_type("x", TimeZone(m_pCsv->get<1>(i)).hour()),
                                                      pair_type("y", m_fn(i))
                                                    })));
    }

    jsonObject.insert("data"  , QJsonValue(QJsonArray({ QJsonValue(jsonDataArray) })));
    jsonObject.insert("labels", QJsonValue(QJsonArray()));
    jsonDocument.setArray(QJsonArray({ QJsonValue(jsonObject) }));

    file.write(jsonDocument.toJson(QJsonDocument::Indented));
    return true;
}
