#include <QtGui/QGuiApplication>
#include <QtGui/QWindow>

#include "hidrawtouchplugin.h"

HidRawTouchPlugin::HidRawTouchPlugin(QObject *parent) :
    QGenericPlugin (parent)
{
}

QObject* HidRawTouchPlugin::create(const QString &name, const QString &spec)
{
    if (name.compare(QLatin1String("hidrawtouch"), Qt::CaseInsensitive) == 0)
        return new HidRawHandler(spec);
    return nullptr;
}

HidRawHandler::HidRawHandler(const QString &spec) :
    m_device (new QTouchDevice)
{
    m_device->setName(QStringLiteral("HIDRAW"));
    m_device->setType(QTouchDevice::TouchScreen);
    m_device->setCapabilities(QTouchDevice::Position);
    QWindowSystemInterface::registerTouchDevice(m_device);
}

void HidRawHandler::processRawPacket(const QByteArray &data)
{
    QWindow *window = QGuiApplication::focusWindow();
    QList<QWindowSystemInterface::TouchPoint> tpoints;
    QWindowSystemInterface::handleTouchEvent(window, m_device, tpoints);
}
