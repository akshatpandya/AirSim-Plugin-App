#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include "common/mavlink.h"
#include "mavlinkparser.h"
#include <QTcpSocket>
#include <QObject>
#include <QByteArray>
#include <QDebug>
#include <QTimer>

class ConnectionManager: public QObject
{
    Q_OBJECT

public slots:
    void connection(QTcpSocket* _socket);

private slots:
    void listenHEARTBEAT(QTcpSocket* _socket);
    void sendHEARTBEAT(QTcpSocket* _socket, uint8_t system_id, uint8_t comp_id, uint8_t base_mode, uint32_t custom_mode, uint8_t system_state);
    void onTimeOut();
};

#endif // CONNECTIONMANAGER_H
