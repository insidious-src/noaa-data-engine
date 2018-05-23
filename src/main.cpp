#include <noaa/dataengine.h>
#include <noaa/ipcinterface.h>
#include <QtCore/QCoreApplication>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    DataEngine(nullptr, "009", "24", "24.01", "43", "43.01");

    QDBusConnection connection = QDBusConnection::sessionBus();
    connection.registerService("org.SolarPower");

    IPCInterface iface(connection);

    return app.exec();
}
