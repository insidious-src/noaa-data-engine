#include <noaa/dataengine.h>
#include <QtCore/QDebug>

namespace {

//http://nomads.ncep.noaa.gov/cgi-bin/filter_gfs_0p25_1hr.pl?
//file=gfs.t06z.pgrb2full.0p50.f009&
//all_lev=on&var_DSWRF=on&var_TMP=on&var_TCDC=on&
//leftlon=24.5&rightlon=24.51&toplat=43&bottomlat=43.01&
//dir=%2Fgfs.2018032706

static const QString part1      = "http://nomads.ncep.noaa.gov/cgi-bin/filter_gfs_0p25_1hr.pl?file=gfs.t00z.pgrb2.0p25.f";
static const QString start_vars = "&lev_middle_cloud_layer=on&lev_surface=on&var_DSWRF=on&var_TMP=on&var_TCDC=on";
static const QString leftlon    = "&leftlon="   ;
static const QString rightlon   = "&rightlon="  ;
static const QString toplat     = "&toplat="    ;
static const QString bottomlat  = "&bottomlat=" ;
static const QString dir        = "&dir=%2Fgfs.";

} // anonymous

DataEngine::DataEngine(QObject* parent            ,
                       const string_type& hrs_fwrd,
                       const string_type& lonleft ,
                       const string_type& lonright,
                       const string_type& lattop  ,
                       const string_type& latbottom)
: QObject(parent),
  m_url(urlImplode(date (), hrs_fwrd, { lonleft, lonright, lattop, latbottom })),
  m_lonleft    (lonleft)  ,
  m_lonright   (lonright) ,
  m_lattop     (lattop)   ,
  m_latbottom  (latbottom),
  m_gWGrib2Proc(this)
{
    download();
}

bool DataEngine::download()
{
    connect(&m_mgr, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));

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
        qDebug() << "wgrib2 convertor not found!";
        return false;
    }

    m_gWGrib2Proc.start("wgrib2 grib2 -undefine out-box " + m_lonleft + ":" +
                        m_lonright + " " + m_lattop + ":" + m_latbottom + " -csv grib2.csv");
    m_gWGrib2Proc.waitForFinished();

    return m_gWGrib2Proc.exitCode() == 0;
}

void DataEngine::replyFinished(QNetworkReply* reply)
{
    if(reply->error())
    {
        qDebug() << "ERROR!";
        qDebug() << reply->errorString();
    }
    else
    {
        qDebug() << reply->header(QNetworkRequest::ContentTypeHeader)  .toString   ();
        qDebug() << reply->header(QNetworkRequest::LastModifiedHeader) .toDateTime ().toString();;
        qDebug() << reply->header(QNetworkRequest::ContentLengthHeader).toULongLong();

        qDebug() << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute  ).toInt   ();
        qDebug() << reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();

        // isolation brackets
        {
            QFile file("grib2");

            if(file.exists()) file.remove();

            if(file.open(QFile::Append))
            {
                file.write(reply->readAll());
                file.flush();
                file.close();
                convert   ();
            }
        }
    }

    reply->deleteLater();
    exit(0);
}

DataEngine::string_type DataEngine::urlImplode(const string_type& start_date,
                                               const string_type& hrf,
                                               const QList<string_type>& coord)
{
    return string_type (part1 + hrf +
                        start_vars  + leftlon  + coord[0] +
                        rightlon    + coord[1] + toplat   + coord[2] + bottomlat + coord[3] +
                        dir         + start_date);
}
