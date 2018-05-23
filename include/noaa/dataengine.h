#ifndef DATAENGINE_H
#define DATAENGINE_H

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtCore/QProcess>
#include <QtCore/QTime>
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

    DataEngine(QObject* parent            ,
               const string_type& hrs_fwrd,
               const string_type& lonleft ,
               const string_type& lonright,
               const string_type& lattop  ,
               const string_type& latbottom);

    bool download();
    bool convert ();

    string_type urlToString() const { return m_url.toString(); }

    static string_type urlImplode (const string_type& start_date,
                                   const string_type& hrf,
                                   const QList<string_type>& coord);

    static string_type date() { return QDate::currentDate().toString("yyyyMMdd") + "00"; }
    static string_type hour() { return QTime::currentTime().toString("hh");              }

public slots:
    void replyFinished(QNetworkReply* m_reply);

private:
    QNetworkAccessManager m_mgr { this };
    QUrl                  m_url;
    string_type           m_lonleft, m_lonright, m_lattop, m_latbottom;
    QProcess              m_gWGrib2Proc;
};


#endif // DATAENGINE_H
