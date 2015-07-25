#ifndef UDEVHELPER_H
#define UDEVHELPER_H

#include <QtCore/QObject>

struct udev;
struct udev_monitor;
class QSocketNotifier;

class UdevHelper : public QObject
{
    Q_OBJECT
public:
    explicit UdevHelper(QObject *parent = 0);
    ~UdevHelper();

    struct DevInfo {
        QString node;
        QString vid;
        QString pid;
    };

    QList<DevInfo> scanConnectedDevices();

signals:
    void deviceDetected(const QString &node, const QString &vid, const QString &pid);
    void deviceRemoved(const QString &node);

private slots:
    void onSocketActivated(int socket_fd);

private:
    udev *m_udev;
    udev_monitor *m_monitor;
    QSocketNotifier *m_socket;
};

#endif // UDEVHELPER_H
