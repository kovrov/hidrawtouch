#include <QtCore/QtEndian>
#include <QtCore/QSocketNotifier>
#include <QtGui/QGuiApplication>
#include <QtGui/QWindow>

#include "udevhelper.h"
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
    m_udev (new UdevHelper(this)),
    m_device (nullptr),
    m_file (nullptr)

{
    Q_UNUSED (spec)
    QObject::connect(m_udev, &UdevHelper::deviceDetected, this, &HidRawHandler::onDeviceDetected);
    QObject::connect(m_udev, &UdevHelper::deviceRemoved, this, &HidRawHandler::onDeviceRemoved);

    for (const auto &i : m_udev->scanConnectedDevices()) {
        if (i.vid.compare("0EEF", Qt::CaseInsensitive) == 0 && i.pid.compare("0005", Qt::CaseInsensitive) == 0) {
            addDevice(i.node);
        }
    }
}

void HidRawHandler::onDeviceDetected(const QString &node, const QString &vid, const QString &pid)
{
    if (vid.compare("0EEF", Qt::CaseInsensitive) == 0 && pid.compare("0005", Qt::CaseInsensitive) == 0) {
        addDevice(node);
    }
}

void HidRawHandler::onDeviceRemoved(const QString &node)
{
    Q_UNUSED (node)
    // FIXME: it's not possible un-register/delete QTouchDevice
    delete m_file;
    m_file = nullptr;
}

void HidRawHandler::addDevice(const QString &node)
{
    if (!m_device) {
        m_device = new QTouchDevice;
        m_device->setName(QStringLiteral("HIDRAW"));
        m_device->setType(QTouchDevice::TouchScreen);
        m_device->setCapabilities(QTouchDevice::Position | QTouchDevice::Area |
                                  QTouchDevice::Pressure | QTouchDevice::NormalizedPosition);
        m_device->setMaximumTouchPoints(4);
        QWindowSystemInterface::registerTouchDevice(m_device);
    }

    if (m_file) {
        delete m_file;
        m_file = nullptr;
    }

    m_file = new QFile(node);
    if (!m_file->open(QIODevice::ReadOnly | QIODevice::Unbuffered)) {
        qWarning("Failed to open %s", qPrintable(node));
        return;
    }
    QSocketNotifier *notify = new QSocketNotifier(m_file->handle(), QSocketNotifier::Read, m_file);
    QObject::connect(notify, &QSocketNotifier::activated, this, &HidRawHandler::onSocketActivated);
}

void HidRawHandler::onSocketActivated(int socket)
{
    Q_UNUSED (socket)
    static QByteArray data (25, Qt::Uninitialized);
    int read = m_file->read(data.data(), 25);
    Q_ASSERT (read == 25);

    struct Packet {
        quint8 id;
        quint8 flags;
        quint16 pos[2];
    };
    const Packet *packets = reinterpret_cast<const Packet *>(data.constData());
    static QList<QWindowSystemInterface::TouchPoint> tpoints {{},{},{},{}};

    for (int i = 0; i < 4; ++i) {
        const Packet &raw_point = packets[i];
        qreal x = qFromBigEndian(raw_point.pos[0]);
        qreal y = qFromBigEndian(raw_point.pos[1]);
        if (i % 2) qSwap(x, y);
        QWindowSystemInterface::TouchPoint &point = tpoints[i];
        point.id = i;
        if (packets[1].flags & (1 << i)) {
            point.area = QRectF(x, y, 0, 0);
            point.pressure = 1.0;
            point.state = Qt::TouchPointPressed;
            point.normalPosition = QPointF(x / 800.0, y / 480.0);
        } else {
            point.pressure = 0.0;
            point.state = Qt::TouchPointReleased;
        }
    }

    QWindow *window = QGuiApplication::focusWindow();
    QWindowSystemInterface::handleTouchEvent(window, m_device, tpoints);
}
