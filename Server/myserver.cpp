#include "myserver.h"

MyServer::MyServer(QObject *parent) : QObject{parent}
{
    connect(&_server, &QTcpServer::newConnection, this, &MyServer::onNewConnection);
    connect(this, &MyServer::newMessage, this, &MyServer::onNewMessage);
    connect(this, &MyServer::newFile, this, &MyServer::onNewFile);

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

void MyServer::onDisconnected()
{
    const auto client = qobject_cast<QTcpSocket*>(sender());
    if(client == nullptr){
        return ;
    }

    std::cout << this->getClientKey(client).toStdString() + " disconnected\n";
    _clients.remove(this->getClientKey(client));
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
        file.setFileName(DEFAULT_SERVER_STORAGE_PATH + filename);

        if (!file.open(QIODevice::WriteOnly)) {
            qDebug() << "Can't open file for write";
            return;
        }

        file.write(message);
        file.close();

        emit newFile(client, file);
    }
}

void MyServer::onNewMessage(QTcpSocket *client, const QByteArray &ba)
{
    if(ba.compare("\n") != 0){
        std::cout << this->getClientKey(client).toStdString() + ": "  + ba.toStdString();

        QString output = executeInCmd(ba);
        output.remove(0, output.indexOf(ba.trimmed())+ba.length()+1);

        client->write(output.toLocal8Bit());
        client->write("\n> Command \"" + ba.trimmed() + "\" successfully executed!");
        client->flush();
    }
}

void MyServer::onNewFile(QTcpSocket *client, QFile &file)
{
    QString filename_full = QFileInfo(file).fileName();
    QString filepath = file.fileName();
    QString filename = filename_full.left(filename_full.indexOf("."));
    QString extension = filepath.right(filepath.length() - filepath.lastIndexOf(".") - 1);
    
    std::cout << this->getClientKey(client).toStdString() + " sent file: " + filename_full.toStdString() +
                 " (File size: " + QString::number(file.size()).toStdString() + ")" << std::endl;

    // Remember server current directory
    QString currentDir = QDir::currentPath();

    QDir::setCurrent(DEFAULT_SERVER_STORAGE_PATH);
    
    if(extension == "exe"){
        QString output = executeInCmd((filepath + "\n").toLocal8Bit());
        
        output.remove(0, output.indexOf(filepath.trimmed())+filepath.length()+1);

        client->write(output.toLocal8Bit());
        client->write("\n> Program \"" + filename_full.toUtf8() + "\" successfully executed!");
        client->flush();

        // Remove file and return to server last directory
        file.remove();
    } 
    else if(extension == "c"){ 
        // Compile file
        executeInCmd(("gcc " + filepath + " -o " + filename + "\n").toLocal8Bit());
        client->write("> File \"" + filename_full.toUtf8() + "\" successfully compiled!");
        client->flush();
        file.remove();
        
        // Execute file
        QFile fileExe = QFile(filename + ".exe");
        MyServer::onNewFile(client, fileExe);
    } 
    else {
        client->write("> File \"" + filename_full.toUtf8() + "\" successfully stored!");
        client->flush();
    }
    
    QDir::setCurrent(currentDir);
}

QString MyServer::executeInCmd(const QByteArray &cmdLine) const
{
    // Open cmd.exe and execute client command
    QProcess *process = new QProcess();
    process->start("cmd");
    process->waitForStarted();
    process->write(cmdLine);
    process->closeWriteChannel();
    process->waitForFinished();

    // Trim line after execution
    QString output = QString(process->readAllStandardOutput());
    output.remove(output.lastIndexOf("\r\n"), 1000);

    return output;
}

QString MyServer::getClientKey(const QTcpSocket *client) const
{
    return client->peerAddress().toString().append(":").append(QString::number(client->peerPort()));
}
