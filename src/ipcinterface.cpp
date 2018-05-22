#include <ipcinterface.h>

IPCInterface::IPCInterface(QDBusConnection& conn, QObject* parent)
: QDBusInterface("org.SolarPower", "/", QString(), conn, parent)
{

}
