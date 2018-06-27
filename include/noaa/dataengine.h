#ifndef DATAENGINE_H
#define DATAENGINE_H

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtCore/QProcess>
#include <QtCore/QUrl>
#include <QtCore/QDateTime>
#include <QtCore/QFile>
#include <QtCore/QList>


/*http://nomads.ncep.noaa.gov/cgi-bin/filter_gfs.pl?\
file=gfs.t${hr}z.pgrbf${fhr}.grib2&\
lev_500_mb=on&lev_700_mb=on&lev_1000_mb=on&\
var_HGT=on&var_RH=on&var_TMP=on&var_UGRD=on&\
var_VGRD=on&subregion=&leftlon=250&\
rightlon=330&toplat=60&bottomlat=20&\
dir=%2Fgfs.${date}${hr}*/

class DataEngine : public QObject
{
    Q_OBJECT
public:
    typedef QString string_type;

    DataEngine(QDate const&       date    ,
               int                hrs_fwrd,
               const string_type& lonleft ,
               const string_type& lonright,
               const string_type& lattop  ,
               const string_type& latbottom,
               const int          timezone = 2);

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
    void replyFinished(QNetworkReply* m_reply);

private:
    static uint          sm_fileCount;
    QNetworkAccessManager m_mgr { this };
    QDateTime             m_datetime;
    QUrl                  m_url;
    int                   m_timezone;
    string_type           m_lonleft, m_lonright, m_lattop, m_latbottom;
    QProcess              m_gWGrib2Proc;

    string_type urlImplode(const QList<string_type>& coord);
};

#endif // DATAENGINE_H
