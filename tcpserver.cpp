#include "tcpserver.h"

TCPServer::TCPServer()
{
    _server = new QTcpServer;
    _socket = new QTcpSocket;
    heartbeatManager_thread = new QThread;
    _server->listen(QHostAddress::Any, 20001);

    //allow no more than 1 connection
    _server->setMaxPendingConnections(30);

    connect(_server, SIGNAL(newConnection()), this, SLOT(onConnection()));
    loadParams();


    len = 0;
    ARMED = false;
    JOYSTICK_CONTROL = false;
    LAND_PROGRESS = false;
    SYSTEM_ID = 1;
    COMPONENT_ID = 1;
    HB_BASE_MODE = MAV_MODE_FLAG_CUSTOM_MODE_ENABLED;
    HB_CUSTOM_MODE = 4;
    HB_SYSTEM_STATUS = MAV_STATE_STANDBY;
}

TCPServer::~TCPServer()
{
    heartbeatManager_thread->quit();
    heartbeatManager_thread->wait();
}

/*When new client is connected, send HEARTBEAT to confirm connection,
  and start a new thread for continuous HEARTBEAT communication.*/
void TCPServer::onConnection()
{
    qDebug() << "New client connected";
    _socket = _server->nextPendingConnection();

    connect(_socket, SIGNAL(readyRead()), this, SLOT(onRead()));
    connect(_socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(onSocketStateChanged(QAbstractSocket::SocketState)));
    rpc_client = new RPCclient;
    sendHeartbeat();
//    if(rpc_client)
//        return;

//    ConnectionManager *manage_heartbeat_obj = new ConnectionManager;
//    manage_heartbeat_obj->moveToThread(heartbeatManager_thread);
//    heartbeatManager_thread->start();
//    connect(this, SIGNAL(start_heartbeatManager_thread(QTcpSocket*)), manage_heartbeat_obj, SLOT(connection(QTcpSocket*)));
//    emit start_heartbeatManager_thread(_socket);
}

//When client is disconnected, display its state and close the connection.
void TCPServer::onSocketStateChanged(QAbstractSocket::SocketState socketState)
{
    qDebug() << "Socket change";
    qDebug() << socketState;
    if (socketState == QAbstractSocket::UnconnectedState)
    {
        QTcpSocket* sender = static_cast<QTcpSocket*>(QObject::sender());
        qDebug() << "Disconnected";
        sender->disconnect();
        heartbeatManager_thread->terminate();
        heartbeatManager_thread->wait();
        rpc_client->disconnect();
        delete rpc_client;
    }
}

//Whenever a MAVLink packet is recieved, do some action.
void TCPServer::onRead()
{
    QTcpSocket* _socket = static_cast<QTcpSocket*>(QObject::sender());
    QByteArray data = _socket->readAll();
    MavlinkParser parse_Mav_msg_obj;
    uint8_t msgID = parse_Mav_msg_obj.parseMavPackets(data);

    switch (msgID)
    {
    case MAVLINK_MSG_ID_PARAM_REQUEST_LIST:
    {
        //send entire parameter list to the client.
        sendParamsListToClient();
        break;
    }

    case MAVLINK_MSG_ID_HEARTBEAT:
    {
        sendHeartbeat();
        break;
    }

    case MAVLINK_MSG_ID_PARAM_REQUEST_READ:
    {
        mavlink_param_request_read_t recv_msg = parse_Mav_msg_obj.get_msg_param_request_read();
        char *param_id = recv_msg.param_id;
        int16_t param_index = recv_msg.param_index;
        sendParam(param_id, param_index);
    }

    case MAVLINK_MSG_ID_COMMAND_LONG:
    {
        mavlink_command_long_t recv_msg = parse_Mav_msg_obj.get_msg_command_long();
        switch (recv_msg.command)
        {
        case MAV_CMD_COMPONENT_ARM_DISARM:
        {
            int8_t arm_disarm_val = recv_msg.param1;

            if(arm_disarm_val == 1)
            {
                /*here arm authorizer is the arming check function. It checks various conditions and sends
                  arm acknowledgement to the GCS*/
                qDebug() << "-----Recieved ARM command-----";
                arming_check();
            }
            else if(arm_disarm_val == 0)
            {
                qDebug() << "-----Recieved DISARM command-----";
                disarm(recv_msg);
            }
            break;
        }
        case MAV_CMD_NAV_TAKEOFF:
        {
            //handle takeOff command
            sendTakeOff(recv_msg);
            break;
        }
        case MAV_CMD_NAV_LAND_LOCAL:
        {
            //handle land command
            sendLand(recv_msg);
            break;
        }
        case MAV_CMD_NAV_LAND:
        {
            //handle land command
            qDebug() << "Land Recieved";
            sendLand(recv_msg);
            break;
        }
        case MAV_CMD_DO_SET_MODE :
        {
            qDebug() << "MAV_CMD_DO_SET_MODE ";
            break;
        }
        default:
            break;
        }
        break;
    }

    case MAVLINK_MSG_ID_SET_MODE:
    {
        mavlink_set_mode_t recv_msg = parse_Mav_msg_obj.get_msg_set_mode();
        qDebug() << recv_msg.base_mode << ", " << recv_msg.custom_mode << ", " << recv_msg.target_system;
        HB_CUSTOM_MODE = recv_msg.custom_mode;
        HB_BASE_MODE = recv_msg.base_mode;
        if(HB_CUSTOM_MODE == 9)
            land(recv_msg);
        break;
    }

    case MAVLINK_MSG_ID_MANUAL_CONTROL:
    {
        mavlink_manual_control_t recv_msg = parse_Mav_msg_obj.get_msg_manual_control();
        JOYSTICK_CONTROL = true;
        manualControl(recv_msg);
        break;
    }
    default:
        break;
    }
}

//Send parameter list for the vehicle to client.
void TCPServer::sendParamsListToClient()
{
    QVector<Param>::iterator ptr;
    int paramIndex = -1;
    for(ptr = param_list.begin(); ptr < param_list.end(); ptr++)
    {
        qDebug() << "Sending parameters.....";
        paramIndex++;
        Param temp = *ptr;
        mavlink_message_t parameter_value_msg;
        QByteArray param_id = temp.param_id;
        if(param_id.size() < 16)
            param_id = param_id.append("\0");
        float_t param_value = temp.param_value;
        mavlink_msg_param_value_pack(SYSTEM_ID, COMPONENT_ID, &parameter_value_msg, (const char*)param_id, param_value, MAV_PARAM_TYPE_REAL32, param_list.size()-1, paramIndex);
        len = mavlink_msg_to_send_buffer(buf, &parameter_value_msg);
        _socket->write((const char*)buf, len);
//        usleep(2000);
    }
}

//Send HEARTBEAT to the client.
void TCPServer::sendHeartbeat()
{
    qDebug() << "Sending heartbeat.....";

    if(rpc_client)
    {
        MultirotorState uav_state = getMultirotorState();
        qDebug() << int(uav_state.landed_state);

        //if the UAV landing is in progress, check if it has landed.
        //if landed, switch the flag, disarm the drone and switch to guided mode.
        if(LAND_PROGRESS == true)
        {
            if(int(uav_state.landed_state) == 0)
            {
                LAND_PROGRESS = false;
                HB_CUSTOM_MODE = 4;
                disarm();
            }
        }
        if(int(uav_state.landed_state) == 1)
        {
            ARMED = true;
        }
    }

    //If custom mode is guided or guided_noGPS, enable 8 in the base mode
    HB_BASE_MODE = MAV_MODE_FLAG_CUSTOM_MODE_ENABLED;

    if(HB_CUSTOM_MODE == 4 || HB_CUSTOM_MODE == 20)
        HB_BASE_MODE |= MAV_MODE_FLAG_GUIDED_ENABLED;
    else if(HB_CUSTOM_MODE == 9)
        HB_BASE_MODE = MAV_MODE_FLAG_CUSTOM_MODE_ENABLED;

    //If manual support enabled, enable 64 in base mode
    //Currently assuming manual mode is enabled.
    HB_BASE_MODE |= MAV_MODE_FLAG_MANUAL_INPUT_ENABLED;

    //If armed, enable 128 in base mode
    if(ARMED)
    {
        HB_BASE_MODE |= MAV_MODE_FLAG_SAFETY_ARMED;
        HB_SYSTEM_STATUS = MAV_STATE_ACTIVE;
    }
    //if unarmed
    else
    {
         HB_SYSTEM_STATUS = MAV_STATE_STANDBY;
    }

    mavlink_message_t msg;
    mavlink_msg_heartbeat_pack(SYSTEM_ID, COMPONENT_ID, &msg, MAV_TYPE_QUADROTOR, MAV_AUTOPILOT_ARDUPILOTMEGA, HB_BASE_MODE, HB_CUSTOM_MODE, HB_SYSTEM_STATUS);
    len = mavlink_msg_to_send_buffer(buf, &msg);
    _socket->write((const char*)buf, len);
}

void TCPServer::sendParam(char* param_id, int16_t param_index)
{
    qDebug() << "Sending parameter.....";
    mavlink_message_t parameter_value_msg;
    float_t param_value = param_list[param_index].param_value;
    mavlink_msg_param_value_pack(SYSTEM_ID, COMPONENT_ID, &parameter_value_msg, param_id, param_value, MAV_PARAM_TYPE_REAL32, TOTAL_PARAMS, param_index);
    len = mavlink_msg_to_send_buffer(buf, &parameter_value_msg);
    _socket->write((const char*)buf, len);
}

void TCPServer::loadParams()
{
    QFile paramFile("../Airsim_plugin_app/mav.txt");
    if (!paramFile.open(QIODevice::ReadOnly))
    {
        qDebug() << "Error in opening the file\n";
        return;
    }
    int paramIndex = 0;
    while (!paramFile.atEnd())
    {
        paramIndex++;
        QByteArray line = paramFile.readLine();
        QList<QByteArray> fields = line.split(',');
        Param temp;
        temp.param_id = fields.first();
        int dataSize = fields.last().size();
        fields.last().truncate(dataSize-1);
        temp.param_value = fields.last().toFloat();
        param_list.push_back(temp);
        param_id_list.push_back(fields.first());
    };
    TOTAL_PARAMS = paramIndex;
    paramFile.close();
}

void TCPServer::arming_check()
{
    if(JOYSTICK_CONTROL == true || LAND_PROGRESS == true)
        return;
    int16_t param_arming_check_index = param_id_list.indexOf("ARMING_CHECK");
    if(param_list[param_arming_check_index].param_value == 0)
    {
        //send arming authorization as accepted
        qDebug() << "Arming.....";
        sendCommandAck(MAV_CMD_COMPONENT_ARM_DISARM);
        ARMED = true;
        rpc_client->arm_disarm(true);
    }
    else if(param_list[param_arming_check_index].param_value == 1)
    {
        //check for the arming conditions
    }
}

void TCPServer::sendCommandAck(uint16_t command)
{
    if(JOYSTICK_CONTROL == true || LAND_PROGRESS == true)
        return;
    mavlink_message_t command_ack_msg;
    mavlink_msg_command_ack_pack(SYSTEM_ID,COMPONENT_ID,&command_ack_msg,command,MAV_RESULT_ACCEPTED,0,20,255,0);
    len = mavlink_msg_to_send_buffer(buf, &command_ack_msg);
    _socket->write((const char*)buf, len);
}

void TCPServer::arm()
{
    if(JOYSTICK_CONTROL == true || LAND_PROGRESS == true)
        return;
    qDebug() << "Arming.....";
    mavlink_message_t command_long_msg;
    mavlink_msg_command_long_pack(SYSTEM_ID, COMPONENT_ID, &command_long_msg, 255, 0, MAV_CMD_COMPONENT_ARM_DISARM, 0, 1,0,0,0,0,0,0);
    len = mavlink_msg_to_send_buffer(buf, &command_long_msg);
    _socket->write((const char*)buf, len);
    ARMED = true;
}

void TCPServer::disarm(mavlink_command_long_t command_msg)
{
    if(JOYSTICK_CONTROL == true || LAND_PROGRESS == true)
        return;
    qDebug() << "Disarming.....";
    ARMED = false;
    rpc_client->arm_disarm(false);
    sendCommandAck(MAV_CMD_COMPONENT_ARM_DISARM);
}

void TCPServer::disarm()
{
    if(JOYSTICK_CONTROL == true || LAND_PROGRESS == true)
        return;
    qDebug() << "Disarming.....";
    ARMED = false;
    rpc_client->arm_disarm(false);
}

void TCPServer::sendTakeOff(mavlink_command_long_t command_msg)
{
    if(JOYSTICK_CONTROL == true || LAND_PROGRESS == true)
        return;
    if(ARMED == false)
    {
        arm();
        rpc_client->arm_disarm(true);
    }
    qDebug() << "Take Off.....";
    sendCommandAck(MAV_CMD_NAV_TAKEOFF);
    rpc_client->takeOff(100);
}

void TCPServer::sendLand(mavlink_command_long_t command_msg)
{
    if(JOYSTICK_CONTROL == true || LAND_PROGRESS == true)
        return;
    qDebug() << "Landing.....";
    sendCommandAck(command_msg.command);
    rpc_client->land(100);
    LAND_PROGRESS = true;
}

void TCPServer::land(mavlink_set_mode_t set_mode_msg)
{
    if(JOYSTICK_CONTROL == true || LAND_PROGRESS == true)
        return;
    qDebug() << "Landing.....";
    rpc_client->land(100);
    LAND_PROGRESS = true;
//    HB_CUSTOM_MODE = 4;
}

MultirotorState TCPServer::getMultirotorState()
{
    MultirotorState uav_state = rpc_client->getMultirotorState();
    return uav_state;
}

void TCPServer::manualControl(mavlink_manual_control_t manual_control_msg)
{
    qDebug() << "Pitch: " << manual_control_msg.x;
    qDebug() << "Roll: " << manual_control_msg.y;
    qDebug() << "Thrust: " << manual_control_msg.z;
    qDebug() << "Yaw: " << manual_control_msg.r;
    if(abs(manual_control_msg.x)<=10 && abs(manual_control_msg.y)<=10 && abs(manual_control_msg.z - 500)<=10 && abs(manual_control_msg.r)<=10)
    {
        JOYSTICK_CONTROL = false;
    }
    else
    {
        JOYSTICK_CONTROL = true;
        RCData joystick_packet;
        QDateTime current = QDateTime::currentDateTime();
        TTimePoint timestamp = current.toTime_t();
        joystick_packet.timestamp = timestamp;
        joystick_packet.pitch = manual_control_msg.x/1000;
        joystick_packet.roll = manual_control_msg.y/1000;
        joystick_packet.throttle = (manual_control_msg.z-500)/500;
        joystick_packet.yaw = manual_control_msg.r/1000;
        joystick_packet.is_initialized = true;
        joystick_packet.is_valid = true;
        rpc_client->moveByRC(joystick_packet);
//        rpc_client->moveByRCYaw(joystick_packet);
    }

}
