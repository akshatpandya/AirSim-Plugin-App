#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QTcpServer>
#include <QByteArray>
#include <QTcpSocket>
#include <QThread>
#include <QFile>
#include <QDateTime>
#include "common/mavlink.h"
#include "mavlinkparser.h"
#include "connectionmanager.h"
#include "rpcclient.h"


class TCPServer: public QObject
{
    Q_OBJECT

public:
    TCPServer();
    ~TCPServer();
    void sendParamsListToClient();
    void sendHeartbeat();
    void sendParam(char* param_id, int16_t param_index);
    void loadParams();
    void arming_check();
    void sendCommandAck(uint16_t command);
    void sendTakeOff(mavlink_command_long_t command_msg);
    void sendLand(mavlink_command_long_t command_msg);
    void disarm();
    void disarm(mavlink_command_long_t command_msg);
    void arm();
    void land(mavlink_set_mode_t set_mode_msg);
    void manualControl(mavlink_manual_control_t manual_control_msg);
    MultirotorState getMultirotorState();

    struct Param
    {
        QByteArray param_id;
        float_t param_value;
    };
public slots:
    void onConnection();
    void onSocketStateChanged(QAbstractSocket::SocketState socketState);
    void onRead();

private:
    QTcpServer* _server;
    QTcpSocket *_socket;
    QThread* heartbeatManager_thread;
    QVector<Param> param_list;
    QVector<QString> param_id_list;
    RPCclient *rpc_client;
    int32_t TOTAL_PARAMS;
    QTimer *param_send_timer;
    uint16_t HB_BASE_MODE;
    uint16_t HB_CUSTOM_MODE;
    uint16_t HB_SYSTEM_STATUS;
    uint16_t SYSTEM_ID;
    uint16_t COMPONENT_ID;
    uint8_t buf[2041];
    uint8_t len;
    bool ARMED;
    bool JOYSTICK_CONTROL;
    bool LAND_PROGRESS; //true when landing in progress, else false

signals:
    QTcpSocket* start_heartbeatManager_thread(QTcpSocket*);
};


#endif // TCPSERVER_H
