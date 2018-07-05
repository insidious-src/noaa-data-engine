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

#ifndef DATAENGINE_H
#define DATAENGINE_H

#include <noaa/json.h>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtCore/QAbstractEventDispatcher>
#include <QtCore/QProcess>
#include <QtCore/QUrl>
#include <QtCore/QDateTime>
#include <QtCore/QTimer>
#include <QtCore/QList>
#include <functional>
#include <memory>

inline int msecsTo (const QTime & at)
{
    constexpr const int msecsPerDay = 24 * 60 * 60 * 1000;
    int msecs = QTime::currentTime().msecsTo(at);

    if (msecs < 0) msecs += msecsPerDay;
    return msecs;
}

template <typename Processor>
inline void runAt (QTime const& at, Processor job)
{
    // Timer ownership prevents timer leak when the thread terminates.
    auto timer = std::make_shared<QTimer>(QAbstractEventDispatcher::instance());

    timer->setTimerType(Qt::CoarseTimer);
    timer->start(msecsTo(at));

    auto conn = std::make_shared<QMetaObject::Connection>();

    *conn = QObject::connect(timer.get(), &QTimer::timeout, [job, conn, timer]
    {
        job();
        QObject::disconnect(*conn);
        timer->deleteLater();
    });
}

// ================================================================

union LocationRect
{
    typedef QString string_type;

    float lonLeft, lonRight, latTop, latBottom;
    float coord[4];

    constexpr LocationRect () noexcept
    : coord { }
    { }

    constexpr LocationRect (float lonleft, float lonright, float lattop, float latbottom) noexcept
    : coord { lonleft, lonright, lattop, latbottom }
    { }

    string_type toString (int idx) const
    { return string_type::number (static_cast<double> (coord[idx])); }
};

// ==========================================================

class DataEngine : public QObject
{
    Q_OBJECT
public:
    typedef std::function<void()> func_type  ;
    typedef QString               string_type;

    DataEngine(QDate const&        date    ,
               int                 hrs_fwrd,
               LocationRect const& loc_rect,
               const int           timezone = 2);

    bool download();
    bool convert ();

    string_type urlToString() const noexcept
    { return m_url.toString(); }

    string_type dateToString() noexcept
    { return m_datetime.date().toString("yyyyMMdd") + "00"; }

    string_type fileName() noexcept
    { return "grib2." + dateToString() + ".f" + hoursFwdString(m_datetime.time().hour()); }

    string_type fileNameUTC(int zone) noexcept
    { return "grib2." + dateToString() + ".f" + hoursFwdString(m_datetime.time().hour() + zone); }

    static string_type hoursFwdString(int const hour_fwd) noexcept
    {
        return 100 > hour_fwd && hour_fwd >= 10 ? "0"  + QString::number(hour_fwd)               :
                                                hour_fwd < 10 ? "00" + QString::number(hour_fwd) :
                                                QString::number(hour_fwd);
    }

public slots:
    void onReplyFinished(QNetworkReply* reply);

signals:
    void downloadFinished(string_type const& file_path);
    void downloadFailed  ();

private:
    QNetworkAccessManager m_mgr { this };
    QDateTime             m_datetime;
    QUrl                  m_url;
    int                   m_timezone;
    LocationRect          m_locRect;
    QProcess              m_gWGrib2Proc;
    unsigned char         m_uTries;

    string_type urlImplode(LocationRect const& coord);
};

#endif // DATAENGINE_H
