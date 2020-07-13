/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -c UsbFormatterInterface -p dbusinterface/usbformatter_interface usbformatter.xml
 *
 * qdbusxml2cpp is Copyright (C) 2016 The Qt Company Ltd.
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#ifndef USBFORMATTER_INTERFACE_H
#define USBFORMATTER_INTERFACE_H

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>

/*
 * Proxy class for interface com.deepin.filemanager.daemon.UsbFormatter
 */
class UsbFormatterInterface: public QDBusAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    { return "com.deepin.filemanager.daemon.UsbFormatter"; }

public:
    UsbFormatterInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = nullptr);

    ~UsbFormatterInterface();

public Q_SLOTS: // METHODS
    inline QDBusPendingReply<bool> mkfs(const QString &path, const QString &fstype, const QString &label)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(path) << QVariant::fromValue(fstype) << QVariant::fromValue(label);
        return asyncCallWithArgumentList(QStringLiteral("mkfs"), argumentList);
    }

Q_SIGNALS: // SIGNALS
};

namespace com {
  namespace deepin {
    namespace filemanager {
      namespace daemon {
        typedef ::UsbFormatterInterface UsbFormatter;
      }
    }
  }
}
#endif
