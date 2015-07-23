#ifndef HIDRAW_TOUCH_PLUGIN_H
#define HIDRAW_TOUCH_PLUGIN_H

#include <QtGui/QGenericPlugin>
#include <qpa/qwindowsysteminterface.h>

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

private:
    void processRawPacket(const QByteArray &data);

    QTouchDevice *m_device;
};

#endif // HIDRAW_TOUCH_PLUGIN_H
