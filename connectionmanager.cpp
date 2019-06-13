#include "connectionmanager.h"
#include <QAbstractSocket>

//This function listen continuously for HEARTBEAT from the client
void ConnectionManager::listenHEARTBEAT(QTcpSocket* _socket)
{
    QByteArray data = _socket->readAll();
    MavlinkParser obj;
    if(obj.parseMavPackets(data) == 0)
    {
        qDebug() << "Recieved heartbeat";
        uint8_t system_id = obj.getMavSystemID(data);
        uint8_t comp_id = obj.getMavCompID(data);
        mavlink_heartbeat_t recv_msg = obj.get_msg_heartbeat();
        uint8_t base_mode = recv_msg.base_mode;
        uint32_t custom_mode = recv_msg.custom_mode;
        uint8_t system_state = recv_msg.system_status;
        sendHEARTBEAT(_socket, system_id, comp_id, base_mode, custom_mode, system_state);
    }
    else
    {
        //a timer for 5 seconds to wait for HEARTBEAT from client
        QTimer *timer = new QTimer;
        connect(timer, SIGNAL(timeout()), this, SLOT(onTimeOut()));
        timer->start(5000);
    }
}

//This function sends HEARTBEAT to the client
void ConnectionManager::sendHEARTBEAT(QTcpSocket* _socket, uint8_t system_id, uint8_t comp_id, uint8_t base_mode, uint32_t custom_mode, uint8_t system_state)
{
    qDebug() << "Sending heartbeat";
    mavlink_message_t msg;
    uint8_t buf[2041];
    uint16_t len = 0;
    mavlink_msg_heartbeat_pack(system_id, comp_id, &msg, MAV_TYPE_QUADROTOR, MAV_AUTOPILOT_ARDUPILOTMEGA, base_mode, custom_mode, system_state);
    len = mavlink_msg_to_send_buffer(buf, &msg);
    _socket->write((const char*)buf, len);
}

void ConnectionManager::connection(QTcpSocket* _socket)
{
    while(1)
    {
        listenHEARTBEAT(_socket);
    }
}

void ConnectionManager::onTimeOut()
{
    qDebug() << "Found no HEARTBEAT for 5 seconds";
    qDebug() << "Disconnecting..";
    //close server
    exit(0);
}
