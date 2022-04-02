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

    QByteArray message = client->readAll();

    if(message.startsWith("[MSG]")){
        // Remove [MSG]
        message = message.remove(0,5);
        emit newMessage(client, message);
    }
    if(message.startsWith("[FILE]")){

        // Remove [FILE]
        message = message.remove(0,6);

        // Get filename and remove brackets
        message = message.remove(0,1);
        const uint k = message.indexOf("]");
        const QByteArray filename = message.left(k);
        message = message.remove(0, k+1);

        QFile file;
        file.setFileName("E:/TEST/" + filename);

        if (!file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            qDebug() << "Can't open file for write";
            return;
        }

        file.write(message);
        file.close();

        qDebug() << "File size:" << file.size();
        //emit newFile(client, file);
    }

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

        client->write("\n> Command \"" + ba.trimmed() + "\" successfully executed!");
        client->flush();
    }

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
