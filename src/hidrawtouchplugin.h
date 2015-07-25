#ifndef HIDRAW_TOUCH_PLUGIN_H
#define HIDRAW_TOUCH_PLUGIN_H

#include <QtCore/QMap>
#include <QtGui/QGenericPlugin>
#include <qpa/qwindowsysteminterface.h>

class UdevHelper;

class HidRawTouchPlugin : public QGenericPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA (IID QGenericPluginFactoryInterface_iid FILE "hidrawtouch.json")

public:
    HidRawTouchPlugin(QObject *parent = nullptr);
    QObject* create(const QString &name, const QString &spec) override;
};

class HidRawHandler : public QObject
{
    Q_OBJECT

public:
    HidRawHandler(const QString &spec);

private slots:
    void onDeviceDetected(const QString &node, const QString &vid, const QString &pid);
    void onDeviceRemoved(const QString &node);
    void onSocketActivated(int socket);

private:
    void addDevice(const QString &node);

    UdevHelper *m_udev;
    QTouchDevice *m_device;
    QFile *m_file;
};

#endif // HIDRAW_TOUCH_PLUGIN_H
