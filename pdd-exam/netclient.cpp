#include "netclient.h"
#include "databox.h"
#include "programconfig.h"
#include <iostream>
#include <QDataStream>
#include <QTextCodec>
#include <QDebug>

using namespace pdd;

NetClient::NetClient(const QString &servAddr, const uint servPort, QObject *parent)
    : QObject(parent)
    , serverAddr(servAddr)
    , serverPort(servPort)
    , socket(0)
    , cmdCode(-1)
    , questionsCount(0)
    , categoryType(-1) {

    socket = new QTcpSocket(this);

    connect(socket, SIGNAL(connected()), this, SLOT(onConnected()));
    connect(socket, SIGNAL(disconnected()), this, SIGNAL(disconnected()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(onError(QAbstractSocket::SocketError)));
    connect(socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));

    DataBox& box = DataBox::inst();
    connect(&box, SIGNAL(taskStarted()), this, SLOT(sendStartTaskPacket()));
    connect(&box, SIGNAL(questionAnswered(uint,uint,uint,uint)),
            this, SLOT(sendAnswerPacket(uint,uint,uint,uint)));
    connect(&box, SIGNAL(extraTask(uint)), this, SLOT(onExtraTask(uint)));

    if(Config::inst().protocolVersion == 0) {
        connect(&box, SIGNAL(successTask()), this, SLOT(sendEndTaskPackets()));
        connect(&box, SIGNAL(failTask()), this, SLOT(sendEndTaskPackets()));
    } else if(Config::inst().protocolVersion == 1) {
        connect(&box, SIGNAL(successTask()), this, SLOT(onSuccessTask()));
        connect(&box, SIGNAL(failTask()), this, SLOT(onFailTask()));
    }

    socket->connectToHost(serverAddr, serverPort);
}


void NetClient::sendIdentificationPacket() {
    QDataStream out(socket);

    if(Config::inst().protocolVersion == 0) {
        uint id = Config::inst().armNumber;
        out.writeRawData((const char*)&id, 2);

    } else if(Config::inst().protocolVersion == 1) {
        QByteArray msg(3, 0);
        msg[0] = 3;
        msg[1] = 1;
        msg[2] = Config::inst().armNumber;
        out.writeRawData(msg.constData(), msg.size());
    }
}


void NetClient::sendStartTaskPacket() {
    QDataStream out(socket);
    if(Config::inst().protocolVersion == 0) {
        quint8 b = 0;
        out.writeRawData( (const char*)&b, 1);

    } else if(Config::inst().protocolVersion == 1) {
        // Посылаем в этом пакете в поле данных идентификаторы вопросов попавших в задание
//        const std::vector<uint> qids = DataBox::inst().getTaskQuestions();
//        QByteArray msg(2+qids.size(), 0);
//        msg[0] = msg.size();
//        msg[1] = 2;
//        for(int n = 2; n < msg.size(); ++n) {
//            msg[n] = qids[n-2];
//        }
//        out.writeRawData(msg.constData(), msg.size());

        const std::vector<uint> qids = DataBox::inst().getTaskQuestions();
        QByteArray msg(2+2*qids.size(), 0);
        msg[0] = msg.size();
        msg[1] = 2;
        char* ptrMsg = msg.data();
        for(int n = 2; n < msg.size(); n+=2) {
            quint16* ptr = (quint16*)(&ptrMsg[n]);
            *ptr = (quint16)(qids[n-2]);
        }
        out.writeRawData(msg.constData(), msg.size());
    }
}


void NetClient::sendAnswerPacket(const uint qn, const uint qid, const uint an, const uint ran) {
    QDataStream out(socket);
    if(Config::inst().protocolVersion == 0) {
        quint8 questionNumber = qn;
        out.writeRawData( (char*)&questionNumber, sizeof(questionNumber));
        quint16 questionId = qid;
        out.writeRawData( (char*)&questionId, sizeof(questionId));
        quint8 answerNumber = an;
        out.writeRawData( (char*)&answerNumber, sizeof(answerNumber));
        quint8 rightAnswerNumber = ran;
        out.writeRawData( (char*)&rightAnswerNumber, sizeof(rightAnswerNumber));

    } else if(Config::inst().protocolVersion == 1) {
        QByteArray msg(7, 0);
        msg[0] = 7;
        msg[1] = 3;
        msg[2] = qn;
        char* ptrMsg = msg.data();
        quint16* ptr = (quint16*)(&ptrMsg[3]);
        *ptr = (quint16)qid;
        msg[5] = an;
        msg[6] = ran;
        out.writeRawData(msg.constData(), msg.size());
    }
}

//void NetClient::sendAnswerPacketOnTest(const uint qid, const uint an, const uint ran) {
//    QDataStream out(socket);
//    if(Config::inst().protocolVersion == 1) {
//        QByteArray msg(6, 0);
//        msg[0] = 6;
//        msg[1] = 6;
//        char* ptrMsg = msg.data();
//        quint16* ptr = (quint16*)(&ptrMsg[2]);
//        *ptr = (quint16)qid;
//        msg[4] = an;
//        msg[5] = ran;
//        out.writeRawData(msg.constData(), msg.size());
//    }
//}


void NetClient::onExtraTask(uint count) {
    QDataStream out(socket);
    if(Config::inst().protocolVersion == 0) {
        quint8 b = 0;
        out.writeRawData( (const char*)&b, 1);

    } else if(Config::inst().protocolVersion == 1) {
        QByteArray msg(3, 0);
        msg[0] = 3;
        msg[1] = 4;
        msg[2] = count;
        out.writeRawData(msg.constData(), msg.size());
    }
}


void NetClient::sendEndTaskPackets() {
    QDataStream out(socket);
    //const DataBox::questions_array& questions = DataBox::inst().getQuestions();
    const uint count = DataBox::inst().getTaskQuestionsCount();
    for(uint n = 0; n < count; ++n) {
        if(DataBox::inst().getAnswer(n) == -1) {
            const question& quest = DataBox::inst().getTaskQuestion(n);
            sendAnswerPacket(n, quest.get_id(), 255, quest.get_answer());
        }
    }
    quint8 b = 0;
    out.writeRawData( (const char*)&b, 1);
}

void NetClient::onSuccessTask() {
    QDataStream out(socket);
    QByteArray msg(3, 0);
    msg[0] = 3;
    msg[1] = 5;
    msg[2] = 0;
    out.writeRawData(msg.constData(), msg.size());
}

void NetClient::onFailTask() {
    QDataStream out(socket);
    QByteArray msg(3, 0);
    msg[0] = 3;
    msg[1] = 5;
    msg[2] = 1;
    out.writeRawData(msg.constData(), msg.size());
}


void NetClient::onError(QAbstractSocket::SocketError ) {
    emit disconnected();
    QTimer::singleShot(0, this, SLOT(nextConnection()));
}


void NetClient::onReadyRead() {

    QDataStream in(socket);

    //qDebug() << "Avialible " << socket->bytesAvailable() << " bytes";

    if(Config::inst().protocolVersion == 0) { // Протокол версии 0

        if(cmdCode == -1) {
            if(socket->bytesAvailable() < (int)sizeof(quint32))
                return;
            in.readRawData( (char*)&cmdCode, sizeof(quint32) );
            //qDebug() << "CMD " << cmdCode;
        }

        uint sz = socket->bytesAvailable();

        if( cmdCode == 0 ) {

            cmdCode = -1;

        } else if( cmdCode == 1 || cmdCode == 2 ) {

            if(questionsCount == 0 && sz >= 1) {
                in.readRawData((char*)&questionsCount, 1);
                //qDebug() << "Questions qount = " << int(questionsCount);
                sz = socket->bytesAvailable();
            }

            if(categoryType == -1 && sz >=4) {
                in.readRawData((char*)&categoryType, 4);
                //qDebug() << "Category " << (categoryType == 1 ? "CD" : "AB");
                sz = socket->bytesAvailable();
            }

            if(userName.isEmpty() && sz > 0) {
                const uint bytes = sz;
                QByteArray name(bytes, 0);
                in.readRawData(name.data(), bytes);

                QTextCodec* codec = QTextCodec::codecForName("UTF-16");
                userName = codec->toUnicode(name);
                //qDebug() << "Name " << userName;
                cmdCode = -1;

                emit registerTask(userName, questionsCount, (categoryType==0 ? true : false));
            }

        } else if(cmdCode == 3) {
            userName.clear();
            categoryType = -1;
            questionsCount = 0;
            cmdCode = -1;
            emit stopTask();
        }

    } else if(Config::inst().protocolVersion == 1) { // Протокол версии 1
        uint sz = socket->bytesAvailable();
        if(sz < 2*sizeof(quint8)) return;

        quint8 packetSize;
        in.readRawData( (char*)&packetSize, sizeof(quint8) );

        quint8 packetId;
        in.readRawData( (char*)&packetId, sizeof(quint8) );

        if(packetId == 1) { // Регистрация задания - экзамен
            in.readRawData( (char*)&questionsCount, sizeof(quint8) );
            quint8 category = 0;
            in.readRawData( (char*)&category, sizeof(quint8) );
            categoryType = category;

            QByteArray name(packetSize-4, 0);
            in.readRawData(name.data(), packetSize-4);
            QTextCodec* codec = QTextCodec::codecForName("UTF-16");
            userName = codec->toUnicode(name);
            emit registerTask(userName, questionsCount, ((categoryType==0 || categoryType==1) ? true : false));
            //emit registerTest(userName, 1, 1, 5, true); // принудительная отладка теста по вопросам

        } else if(packetId == 2) { // Окончание задания
            userName.clear();
            categoryType = -1;
            questionsCount = 0;
            emit stopTask();

        } else if(packetId == 3) { // Регистрация задания - тест
            questionsCount = 0;
            quint8 th_n = 0;
            in.readRawData( (char*)&th_n, sizeof(quint8) );
            quint8 quest_n = 0;
            in.readRawData( (char*)&quest_n, sizeof(quint8) );
            quint16 qcount = 0;
            in.readRawData( (char*)&qcount, sizeof(quint16) );
            quint8 category = 0;
            in.readRawData( (char*)&category, sizeof(quint8) );
            categoryType = category;

            QByteArray name(packetSize-7, 0);
            in.readRawData(name.data(), packetSize-7);
            QTextCodec* codec = QTextCodec::codecForName("UTF-16");
            userName = codec->toUnicode(name);
            emit registerTest(userName, th_n, quest_n, qcount,
                              ((categoryType==0 || categoryType==1) ? true : false));
        }
    }
}


void NetClient::onConnected() {
    sendIdentificationPacket();
    emit connected();
}


void NetClient::nextConnection() {
    socket->abort();
    socket->connectToHost( serverAddr, serverPort );
}
