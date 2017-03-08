#ifndef NETCLIENT_H
#define NETCLIENT_H

#include <QObject>
#include <QTcpSocket>

namespace pdd {

class NetClient : public QObject {
    Q_OBJECT
public:
    explicit NetClient(const QString& servAddr, const uint servPort, QObject *parent = 0);

signals:
    void connected();
    void disconnected();
    void registerTask(QString name, uint qcount, bool category_ab);
    void stopTask();

public slots:
    /** Отправка пакета с идентификационными данными клиента */
    void sendIdentificationPacket();

    /** Отправка пакета с командой НАЧАТЬ ТЕСТ */
    void sendStartTaskPacket();

    /** Отправка пакета с ответом пользователя на очередной вопрос экзамена */
    void sendAnswerPacket(const uint qn, const uint qid, const uint an, const uint ran);

    /** Отправка пакета с ответом пользователя на очередной вопрос теста */
    void sendAnswerPacketOnTest(const uint qid, const uint an, const uint ran);

    void onExtraTask(uint count);

    /** Отправка пакета закончен/не закончен тест. "Родной" протокол обмена. */
    void sendEndTaskPackets();

    /** Отправка пакета экзамен сдан. */
    void onSuccessTask();

    /** Отправка пакета экзамен не сдан. */
    void onFailTask();

private slots:

    void onError(QAbstractSocket::SocketError);
    void onReadyRead();
    void onConnected();
    void nextConnection();

private:
    QString serverAddr;
    uint serverPort;
    QTcpSocket* socket;
    int cmdCode;
    uint questionsCount;
    int categoryType;
    QString userName;
};

}

#endif // NETCLIENT_H
