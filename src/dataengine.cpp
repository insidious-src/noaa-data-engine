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

#include <noaa/dataengine.h>
#include <noaa/csv.h>
#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <iostream>
#include <thread>
#include <chrono>

namespace {

static const QString part1      = "http://nomads.ncep.noaa.gov/cgi-bin/filter_gfs_0p25_1hr.pl?file=gfs.t00z.pgrb2.0p25.f";
//static const QString start_vars = "&lev_middle_cloud_layer=on&lev_surface=on&var_DSWRF=on&var_TMP=on&var_TCDC=on";
static const QString start_vars = "&var_DSWRF=on";
static const QString leftlon    = "&leftlon="   ;
static const QString rightlon   = "&rightlon="  ;
static const QString toplat     = "&toplat="    ;
static const QString bottomlat  = "&bottomlat=" ;
static const QString dir        = "&dir=%2Fgfs.";

constexpr const auto ConnectionRetries = 3U;

} // anonymous

// ==========================================================

DataEngine::DataEngine(QDate const&        date    ,
                       int                 hrs_fwrd,
                       LocationRect const& loc_rect,
                       const int           timezone)
: QObject      (),
  m_datetime   (date, QTime(hrs_fwrd, 0)),
  m_url        (urlImplode(loc_rect)),
  m_timezone   (timezone),
  m_locRect    (loc_rect),
  m_gWGrib2Proc(this),
  m_uTries     ()
{
    download();
}

bool DataEngine::download()
{
    connect(&m_mgr, SIGNAL(finished(QNetworkReply*)), this, SLOT(onReplyFinished(QNetworkReply*)));

    if (m_uTries >= ConnectionRetries) m_uTries = 0;

    return m_mgr.get(QNetworkRequest(m_url))->isFinished();
}

bool DataEngine::convert()
{
#   ifdef Q_OS_UNIX
    QFile file("/usr/bin/wgrib2");
#   else
    QFile file("C:\Program Files\wgrib2\wgrib2");
#   endif

    if(!file.exists())
    {
        std::cout << "wgrib2 convertor not found!";
        return false;
    }

    m_gWGrib2Proc.start("wgrib2 db/" + fileNameUTC(m_timezone) + " -undefine out-box " +
                        m_locRect.toString(0) + ":" + m_locRect.toString(1) + " "   +
                        m_locRect.toString(2) + ":" + m_locRect.toString(3) +
                        " -csv db/" + fileNameUTC(m_timezone) + ".csv");
    m_gWGrib2Proc.waitForFinished();

    auto ret = m_gWGrib2Proc.exitCode() == 0;

    if (ret) std::cout << fileNameUTC(m_timezone).toStdString() << " converted with time zone UTC"
                       << (m_timezone >= 0 ? " + " : " - ") << std::to_string(m_timezone) << std::endl;
    return ret;
}

void DataEngine::onReplyFinished(QNetworkReply* reply)
{
    switch (reply->error())
    {
    case QNetworkReply::NoError:
        std::cout << m_url.toString().toStdString() << " successfully downloaded\n";
        std::cout << reply->header(QNetworkRequest::ContentTypeHeader).toString().toStdString() << std::endl;

        // isolation brackets
        {
            QFile file("db/" + fileNameUTC(m_timezone));

            if(file.open(QFile::Truncate | QFile::WriteOnly))
            {
                file.write(reply->readAll());
                file.flush();
                file.close();
                convert   ();

                emit downloadFinished(file.fileName());
            }
        }
        break;
    default:
        std::cout << "ERROR!" << std::endl;
        std::cout << reply->errorString().toStdString() << std::endl;
    case QNetworkReply::HostNotFoundError: case QNetworkReply::TimeoutError:
        std::cout << "\nRetrying connection...\n";

        if(m_uTries++ < ConnectionRetries)
        {
            std::this_thread::sleep_for(std::chrono::seconds(3));
            download();
        }
        else emit downloadFailed();
        break;
    }

    reply->deleteLater();
}

DataEngine::string_type DataEngine::urlImplode(LocationRect const& rect)
{
    return string_type (part1      + hoursFwdString(m_datetime.time().hour()) +
                        start_vars + leftlon  + rect.toString(0) +
                        rightlon   + rect.toString(1) +
                        toplat     + rect.toString(2) +
                        bottomlat  + rect.toString(3) +
                        dir        + dateToString());
}
