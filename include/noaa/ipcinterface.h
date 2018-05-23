#ifndef IPCINTERFACE_H
#define IPCINTERFACE_H

#include <QtDBus/QtDBus>

class IPCInterface : public QDBusInterface
{
    Q_OBJECT
public:
    explicit IPCInterface(QDBusConnection& conn, QObject* parent = nullptr);

signals:

public slots:

private:
};

#endif // IPCINTERFACE_H
