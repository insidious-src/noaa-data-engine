#include <noaa/dataengine.h>
#include <noaa/ipcinterface.h>
#include <QtCore/QCoreApplication>
#include <QtCore/QEventLoop>
#include <memory>

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    std::shared_ptr<DataEngine> data[16];

    for(auto i = 0, n = 4; n < 20; ++n, ++i)
    {
        data[i].reset(new DataEngine(QDate(2018, 6, 26), n, "24.5", "24.5", "43.25", "43.25"));
    }

    //QDBusConnection connection = QDBusConnection::sessionBus();
    //connection.registerService("org.SolarPower");

    //IPCInterface iface(connection);

    return app.exec();
}
