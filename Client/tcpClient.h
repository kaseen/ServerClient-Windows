#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <iostream>
#include <QObject>
#include <QTcpSocket>
#include <QFile>
#include <QFileInfo>

class TcpClient : public QObject
{
    Q_OBJECT
public:
    explicit TcpClient(QObject *parent = nullptr);

signals:
    void newMessage(const QByteArray &ba);

public slots:
    void connectToHost(const QString &ip, const QString &port);
    void sendMessage(const QString &message);
    void sendFile(const QString &path);

private slots:
    void onConnected();
    void onErrorOccurred(QAbstractSocket::SocketError error);
    void onReadyRead();

private:
    QTcpSocket _socket;
};

#endif // TCPCLIENT_H
