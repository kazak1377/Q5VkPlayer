/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -c DBusAdaptor -a dbusadaptor.h:dbusadaptor.cpp org.mpris.MediaPlayer2.Player.xml
 *
 * qdbusxml2cpp is Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#include "dbusadaptor.h"
#include <QtCore/QMetaObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

/*
 * Implementation of adaptor class DBusAdaptor
 */

DBusAdaptor::DBusAdaptor(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    // constructor
    setAutoRelaySignals(true);
}

DBusAdaptor::~DBusAdaptor()
{
    // destructor
}

bool DBusAdaptor::canControl() const
{
    // get the value of property CanControl
    return qvariant_cast< bool >(parent()->property("CanControl"));
}

bool DBusAdaptor::canGoNext() const
{
    // get the value of property CanGoNext
    return qvariant_cast< bool >(parent()->property("CanGoNext"));
}

bool DBusAdaptor::canGoPrevious() const
{
    // get the value of property CanGoPrevious
    return qvariant_cast< bool >(parent()->property("CanGoPrevious"));
}

bool DBusAdaptor::canPause() const
{
    // get the value of property CanPause
    return qvariant_cast< bool >(parent()->property("CanPause"));
}

bool DBusAdaptor::canPlay() const
{
    // get the value of property CanPlay
    return qvariant_cast< bool >(parent()->property("CanPlay"));
}

bool DBusAdaptor::canSeek() const
{
    // get the value of property CanSeek
    return qvariant_cast< bool >(parent()->property("CanSeek"));
}

QString DBusAdaptor::loopStatus() const
{
    // get the value of property LoopStatus
    return qvariant_cast< QString >(parent()->property("LoopStatus"));
}

void DBusAdaptor::setLoopStatus(const QString &value)
{
    // set the value of property LoopStatus
    parent()->setProperty("LoopStatus", QVariant::fromValue(value));
}

double DBusAdaptor::maximumRate() const
{
    // get the value of property MaximumRate
    return qvariant_cast< double >(parent()->property("MaximumRate"));
}

QVariantMap DBusAdaptor::metadata() const
{
    // get the value of property Metadata
    return qvariant_cast< QVariantMap >(parent()->property("Metadata"));
}

double DBusAdaptor::minimumRate() const
{
    // get the value of property MinimumRate
    return qvariant_cast< double >(parent()->property("MinimumRate"));
}

QString DBusAdaptor::playbackStatus() const
{
    // get the value of property PlaybackStatus
    return qvariant_cast< QString >(parent()->property("PlaybackStatus"));
}

qlonglong DBusAdaptor::position() const
{
    // get the value of property Position
    return qvariant_cast< qlonglong >(parent()->property("Position"));
}

double DBusAdaptor::rate() const
{
    // get the value of property Rate
    return qvariant_cast< double >(parent()->property("Rate"));
}

void DBusAdaptor::setRate(double value)
{
    // set the value of property Rate
    parent()->setProperty("Rate", QVariant::fromValue(value));
}

bool DBusAdaptor::shuffle() const
{
    // get the value of property Shuffle
    return qvariant_cast< bool >(parent()->property("Shuffle"));
}

void DBusAdaptor::setShuffle(bool value)
{
    // set the value of property Shuffle
    parent()->setProperty("Shuffle", QVariant::fromValue(value));
}

double DBusAdaptor::volume() const
{
    // get the value of property Volume
    return qvariant_cast< double >(parent()->property("Volume"));
}

void DBusAdaptor::setVolume(double value)
{
    // set the value of property Volume
    parent()->setProperty("Volume", QVariant::fromValue(value));
}

void DBusAdaptor::Next()
{
    // handle method call org.mpris.MediaPlayer2.Player.Next
    QMetaObject::invokeMethod(parent(), "Next");
}

void DBusAdaptor::OpenUri(const QString &Uri)
{
    // handle method call org.mpris.MediaPlayer2.Player.OpenUri
    QMetaObject::invokeMethod(parent(), "OpenUri", Q_ARG(QString, Uri));
}

void DBusAdaptor::Pause()
{
    // handle method call org.mpris.MediaPlayer2.Player.Pause
    QMetaObject::invokeMethod(parent(), "Pause");
}

void DBusAdaptor::Play()
{
    // handle method call org.mpris.MediaPlayer2.Player.Play
    QMetaObject::invokeMethod(parent(), "Play");
}

void DBusAdaptor::PlayPause()
{
    // handle method call org.mpris.MediaPlayer2.Player.PlayPause
    QMetaObject::invokeMethod(parent(), "PlayPause");
}

void DBusAdaptor::Previous()
{
    // handle method call org.mpris.MediaPlayer2.Player.Previous
    QMetaObject::invokeMethod(parent(), "Previous");
}

void DBusAdaptor::Seek(qlonglong Offset)
{
    // handle method call org.mpris.MediaPlayer2.Player.Seek
    QMetaObject::invokeMethod(parent(), "Seek", Q_ARG(qlonglong, Offset));
}

void DBusAdaptor::SetPosition(const QDBusObjectPath &TrackId, qlonglong Position)
{
    // handle method call org.mpris.MediaPlayer2.Player.SetPosition
    QMetaObject::invokeMethod(parent(), "SetPosition", Q_ARG(QDBusObjectPath, TrackId), Q_ARG(qlonglong, Position));
}

void DBusAdaptor::Stop()
{
    // handle method call org.mpris.MediaPlayer2.Player.Stop
    QMetaObject::invokeMethod(parent(), "Stop");
}

