/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -c DBusAdaptor1 -a dbusadaptor1.h:dbusadaptor1.cpp org.mpris.MediaPlayer2.xml
 *
 * qdbusxml2cpp is Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 * This file may have been hand-edited. Look for HAND-EDIT comments
 * before re-generating it.
 */

#ifndef DBUSADAPTOR1_H_1392135927
#define DBUSADAPTOR1_H_1392135927

#include <QtCore/QObject>
#include <QtDBus/QtDBus>
QT_BEGIN_NAMESPACE
class QByteArray;
template<class T> class QList;
template<class Key, class Value> class QMap;
class QString;
class QStringList;
class QVariant;
QT_END_NAMESPACE

/*
 * Adaptor class for interface org.mpris.MediaPlayer2
 */
class DBusAdaptor1: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.mpris.MediaPlayer2")
    Q_CLASSINFO("D-Bus Introspection", ""
"  <interface name=\"org.mpris.MediaPlayer2\">\n"
"    <method name=\"Raise\"/>\n"
"    <method name=\"Quit\"/>\n"
"    <property access=\"read\" type=\"b\" name=\"CanQuit\"/>\n"
"    <property access=\"read\" type=\"b\" name=\"CanRaise\"/>\n"
"    <property access=\"read\" type=\"b\" name=\"HasTrackList\"/>\n"
"    <property access=\"read\" type=\"s\" name=\"Identity\"/>\n"
"    <property access=\"read\" type=\"s\" name=\"DesktopEntry\"/>\n"
"    <property access=\"read\" type=\"as\" name=\"SupportedUriSchemes\"/>\n"
"    <property access=\"read\" type=\"as\" name=\"SupportedMimeTypes\"/>\n"
"    <property access=\"readwrite\" type=\"b\" name=\"Fullscreen\"/>\n"
"    <property access=\"read\" type=\"b\" name=\"CanSetFullscreen\"/>\n"
"  </interface>\n"
        "")
public:
    DBusAdaptor1(QObject *parent);
    virtual ~DBusAdaptor1();

public: // PROPERTIES
    Q_PROPERTY(bool CanQuit READ canQuit)
    bool canQuit() const;

    Q_PROPERTY(bool CanRaise READ canRaise)
    bool canRaise() const;

    Q_PROPERTY(bool CanSetFullscreen READ canSetFullscreen)
    bool canSetFullscreen() const;

    Q_PROPERTY(QString DesktopEntry READ desktopEntry)
    QString desktopEntry() const;

    Q_PROPERTY(bool Fullscreen READ fullscreen WRITE setFullscreen)
    bool fullscreen() const;
    void setFullscreen(bool value);

    Q_PROPERTY(bool HasTrackList READ hasTrackList)
    bool hasTrackList() const;

    Q_PROPERTY(QString Identity READ identity)
    QString identity() const;

    Q_PROPERTY(QStringList SupportedMimeTypes READ supportedMimeTypes)
    QStringList supportedMimeTypes() const;

    Q_PROPERTY(QStringList SupportedUriSchemes READ supportedUriSchemes)
    QStringList supportedUriSchemes() const;

public Q_SLOTS: // METHODS
    void Quit();
    void Raise();
Q_SIGNALS: // SIGNALS
};

#endif