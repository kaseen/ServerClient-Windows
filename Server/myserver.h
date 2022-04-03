#ifndef MYSERVER_H
#define MYSERVER_H

#include <iostream>
#include <QObject>
#include <QDebug>
#include <QProcess>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHash>
#include <QDir>
#include <QFile>
#include <QFileInfo>

#define DEFAULT_SERVER_STORAGE_PATH "E:/TEST/"

class MyServer : public QObject
{
    Q_OBJECT
public:
    explicit MyServer(QObject *parent = nullptr);

signals:
    void newMessage(QTcpSocket *client, const QByteArray &message);
    void newFile(QTcpSocket *client, QFile &file);

private slots:
    void onNewConnection();
    void onDisconnected();
    void onReadyRead();
    void onNewMessage(QTcpSocket *client, const QByteArray &ba);
    void onNewFile(QTcpSocket *client, QFile &file);

private:
    QString executeInCmd(const QByteArray &cmdLine) const;
    QString getClientKey(const QTcpSocket *client) const;
    QTcpServer _server;
    QHash<QString, QTcpSocket*> _clients;
};

#endif // MYSERVER_H
