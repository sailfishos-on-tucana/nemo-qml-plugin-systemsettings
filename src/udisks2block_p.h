/*
 * Copyright (C) 2018 Jolla Ltd. <raine.makelainen@jolla.com>
 *
 * You may use this file under the terms of the BSD license as follows:
 *
 * "Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   * Neither the name of Nemo Mobile nor the names of its contributors
 *     may be used to endorse or promote products derived from this
 *     software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
 */

#ifndef UDISKS2_BLOCK_H
#define UDISKS2_BLOCK_H

#include <QObject>
#include <QVariantMap>
#include <QDBusConnection>

#include "udisks2defines.h"

class QDBusPendingCallWatcher;

namespace UDisks2 {

class Block : public QObject
{
    Q_OBJECT

public:
    Block(const QString &path, const UDisks2::InterfacePropertyMap &interfacePropertyMap, QObject *parent = nullptr);
    Block& operator=(const Block& other);

    ~Block();

    QString path() const;

    QString device() const;
    QString preferredDevice() const;
    QString drive() const;

    qint64 deviceNumber() const;
    QString id() const;

    qint64 size() const;

    bool hasCryptoBackingDevice() const;
    QString cryptoBackingDevicePath() const;
    QString cryptoBackingDeviceObjectPath() const;

    bool isEncrypted() const;
    void setEncrypted(bool encrypted);

    bool isMountable() const;
    void setMountable(bool mountable);

    bool isFormatting() const;
    void setFormatting();

    bool isLocking() const;
    void setLocking();

    bool isReadOnly() const;
    bool isExternal() const;

    QString idType() const;
    QString idVersion() const;
    QString idLabel() const;
    QString idUUID() const;

    QString mountPath() const;

    QVariant value(const QString &key) const;

    bool hasData() const;

    void dumpInfo() const;

    static QString cryptoBackingDevicePath(const QString &objectPath);

signals:
    void completed();
    void updated();
    void formatted();
    void mountPathChanged();

private slots:
    void updateProperties(const QDBusMessage &message);

private:
    void updateMountPoint(const QVariant &mountPoints);
    void complete();
    void getFileSystemInterface();
    void getEncryptedInterface();

    QString m_path;
    UDisks2::InterfacePropertyMap m_interfacePropertyMap;
    QVariantMap m_data;
    QDBusConnection m_connection;
    QString m_mountPath;
    bool m_mountable;
    bool m_encrypted;
    bool m_formatting;
    bool m_locking;

    QDBusPendingCallWatcher *m_pendingFileSystem;
    QDBusPendingCallWatcher *m_pendingBlock;
    QDBusPendingCallWatcher *m_pendingEncrypted;
};

}

#endif
