/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -c StartManagerInterface -p startmanager_interface startmanager.xml
 *
 * qdbusxml2cpp is Copyright (C) 2017 The Qt Company Ltd.
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#ifndef STARTMANAGER_INTERFACE_H
#define STARTMANAGER_INTERFACE_H

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>

/*
 * Proxy class for interface com.deepin.StartManager
 */
class StartManagerInterface: public QDBusAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    { return "com.deepin.StartManager"; }

public:
    StartManagerInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = nullptr);

    ~StartManagerInterface();

public Q_SLOTS: // METHODS
    inline QDBusPendingReply<bool> AddAutostart(const QString &in0)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(in0);
        return asyncCallWithArgumentList(QStringLiteral("AddAutostart"), argumentList);
    }

    inline QDBusPendingReply<QStringList> AutostartList()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QStringLiteral("AutostartList"), argumentList);
    }

    inline QDBusPendingReply<bool> IsAutostart(const QString &in0)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(in0);
        return asyncCallWithArgumentList(QStringLiteral("IsAutostart"), argumentList);
    }

    inline QDBusPendingReply<bool> Launch(const QString &in0)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(in0);
        return asyncCallWithArgumentList(QStringLiteral("Launch"), argumentList);
    }

    inline QDBusPendingReply<> LaunchApp(const QString &in0, uint in1, const QStringList &in2)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(in0) << QVariant::fromValue(in1) << QVariant::fromValue(in2);
        return asyncCallWithArgumentList(QStringLiteral("LaunchApp"), argumentList);
    }

    inline QDBusPendingReply<> LaunchAppAction(const QString &in0, const QString &in1, uint in2)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(in0) << QVariant::fromValue(in1) << QVariant::fromValue(in2);
        return asyncCallWithArgumentList(QStringLiteral("LaunchAppAction"), argumentList);
    }

    inline QDBusPendingReply<bool> LaunchWithTimestamp(const QString &in0, uint in1)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(in0) << QVariant::fromValue(in1);
        return asyncCallWithArgumentList(QStringLiteral("LaunchWithTimestamp"), argumentList);
    }

    inline QDBusPendingReply<bool> RemoveAutostart(const QString &in0)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(in0);
        return asyncCallWithArgumentList(QStringLiteral("RemoveAutostart"), argumentList);
    }

    inline QDBusPendingReply<> RunCommand(const QString &in0, const QStringList &in1)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(in0) << QVariant::fromValue(in1);
        return asyncCallWithArgumentList(QStringLiteral("RunCommand"), argumentList);
    }

    inline QDBusPendingReply<> RunCommandWithOptions(const QString &in0, const QStringList &in1, const QVariantMap &in2)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(in0) << QVariant::fromValue(in1) << QVariant::fromValue(in2);
        return asyncCallWithArgumentList(QStringLiteral("RunCommandWithOptions"), argumentList);
    }

Q_SIGNALS: // SIGNALS
    void AutostartChanged(const QString &in0, const QString &in1);
};

namespace com {
  namespace deepin {
    typedef ::StartManagerInterface StartManager;
  }
}
#endif
