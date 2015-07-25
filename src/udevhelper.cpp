#include <QtCore/QStringList>
#include <QtCore/QSocketNotifier>
#include <libudev.h>

#include "udevhelper.h"

UdevHelper::UdevHelper(QObject *parent) :
    QObject (parent),
    m_udev (udev_new()),
    m_monitor (udev_monitor_new_from_netlink(m_udev, "udev")),
    m_socket (nullptr)
{
    udev_monitor_filter_add_match_subsystem_devtype(m_monitor, "hidraw", nullptr);
    udev_monitor_enable_receiving(m_monitor);

    m_socket = new QSocketNotifier(udev_monitor_get_fd(m_monitor), QSocketNotifier::Read, this);
    QObject::connect(m_socket, &QSocketNotifier::activated, this, &UdevHelper::onSocketActivated);
}

UdevHelper::~UdevHelper()
{
    QObject::disconnect(m_socket);
    m_monitor = udev_monitor_unref(m_monitor);
    m_udev = udev_unref(m_udev);
}

void UdevHelper::onSocketActivated(int socket_fd)
{
    Q_UNUSED (socket_fd);

    udev_device *dev = udev_monitor_receive_device(m_monitor);
    if (!dev) {
        qWarning("udev_monitor_receive_device return NULL");
        return;
    }

    const char *action = udev_device_get_action(dev);

    if (qstrcmp(action, "add") == 0) {
        // parent is not referenced
        udev_device *parent = udev_device_get_parent_with_subsystem_devtype(dev, "usb", "usb_device");
        emit deviceDetected(QString::fromUtf8(udev_device_get_devnode(dev)),
                            QString::fromUtf8(udev_device_get_sysattr_value(parent, "idVendor")),
                            QString::fromUtf8(udev_device_get_sysattr_value(parent, "idProduct")));
    }
    else if (qstrcmp(action, "remove") == 0) {
        emit deviceRemoved(QString::fromUtf8(udev_device_get_devnode(dev)));
    }

    udev_device_unref(dev);
}

QList<UdevHelper::DevInfo> UdevHelper::scanConnectedDevices()
{
    udev_enumerate *list = udev_enumerate_new(m_udev);
    udev_enumerate_add_match_subsystem(list, "hidraw");
    udev_enumerate_scan_devices(list);

    QList<DevInfo> res;
    for (auto i = udev_enumerate_get_list_entry(list); i; i = udev_list_entry_get_next(i)) {
        const char *name = udev_list_entry_get_name(i);
        udev_device *device = udev_device_new_from_syspath(m_udev, name);

        udev_device *parent = udev_device_get_parent_with_subsystem_devtype(device, "usb", "usb_device");
        DevInfo info {
            QString::fromUtf8(udev_device_get_devnode(device)),
            QString::fromUtf8(udev_device_get_sysattr_value(parent, "idVendor")),
            QString::fromUtf8(udev_device_get_sysattr_value(parent, "idProduct"))
        };
        res << info;
        udev_device_unref(device);
    }
    return res;
}
