#ifndef MYSERVER_H
#define MYSERVER_H

#include <iostream>
#include <QObject>
#include <QDebug>
#include <QProcess>
#include <QFile>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHash>

class MyServer : public QObject
{
    Q_OBJECT
public:
    explicit MyServer(QObject *parent = nullptr);

signals:
    void newMessage(QTcpSocket *client, const QByteArray &message);

private slots:
    void onNewConnection();
    void onReadyRead();
    void onNewMessage(QTcpSocket *client, const QByteArray &ba);
    void onDisconnected();

private:
    QString getClientKey(const QTcpSocket *client) const;
    QTcpServer _server;
    QHash<QString, QTcpSocket*> _clients;
};

#endif // MYSERVER_H
