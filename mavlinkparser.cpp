#include "mavlinkparser.h"

MavlinkParser::MavlinkParser()
{
    mav_msg_id = 0;
    mav_msg_systemID = 0;
    mav_msg_compID = 0;
}

uint8_t MavlinkParser::parseMavPackets(QByteArray data)
{
    int dataSize = data.size();
    mavlink_message_t msg;
    mavlink_status_t status;

    for(int i=0; i<dataSize; i++)
    {
        if(mavlink_parse_char(MAVLINK_COMM_0, data[i], &msg, &status))
        {
            //MAVLINK message #0
            if(msg.msgid == MAVLINK_MSG_ID_HEARTBEAT)
            {
                //recieved a HEARTBEAT
                qDebug() << "-----Recieved a HEARTBEAT-----";
                mav_msg_id = msg.msgid;
                mavlink_heartbeat_t recv_msg;
                mavlink_msg_heartbeat_decode(&msg, &recv_msg);
                //return HEARTBEAT message
                heartbeat_msg = recv_msg;
                return msg.msgid;
            }
            //MAVLINK message #21
            else if(msg.msgid == MAVLINK_MSG_ID_PARAM_REQUEST_LIST)
            {
                qDebug() << "-----Recieved Parameter List Request-----";
                mav_msg_id = msg.msgid;
                mavlink_param_request_list_t recv_msg;
                mavlink_msg_param_request_list_decode(&msg, &recv_msg);
                //return PARAM_REQUEST_LIST message
                param_request_list_msg = recv_msg;
                return msg.msgid;
            }

            //MAVLINK message #20
            else if(msg.msgid == MAVLINK_MSG_ID_PARAM_REQUEST_READ)
            {
                qDebug() << "-----Recieved Parameter Request Read-----";
                mav_msg_id = msg.msgid;
                mavlink_param_request_read_t recv_msg;
                mavlink_msg_param_request_read_decode(&msg, &recv_msg);
                param_request_read_msg = recv_msg;
                qDebug() << (QByteArray)recv_msg.param_id;
                qDebug() << recv_msg.param_index;
                return msg.msgid;
            }

            //MAVLINK message #43
            else if(msg.msgid == MAVLINK_MSG_ID_MISSION_REQUEST_LIST)
            {
                qDebug() << "-----Recieved Mission Request List-----";
                mav_msg_id = msg.msgid;
                mavlink_mission_request_list_t recv_msg;
                mavlink_msg_mission_request_list_decode(&msg, &recv_msg);
                mission_request_list_msg = recv_msg;
                return msg.msgid;
            }

            //MAVLINK message #44
            else if(msg.msgid == MAVLINK_MSG_ID_MISSION_COUNT)
            {
                qDebug() << "-----Recieved Mission Count-----";
                return msg.msgid;
            }

            //MAVLINK message #75
            else if(msg.msgid == MAVLINK_MSG_ID_COMMAND_ACK)
            {
                qDebug() << "-----Recieved Command Acknowledgement-----";
                mavlink_command_ack_t recv_msg;
                mavlink_msg_command_ack_decode(&msg, &recv_msg);
                command_ack_msg = recv_msg;
                return msg.msgid;
            }

            //MAVLINK message #76
            else if(msg.msgid == MAVLINK_MSG_ID_COMMAND_LONG)
            {
                qDebug() << "-----Recieved Command Long-----";
                mavlink_command_long_t recv_msg;
                mavlink_msg_command_long_decode(&msg, &recv_msg);
                command_long_msg = recv_msg;
                return msg.msgid;
            }

            //MAVLINK message #11
            else if(msg.msgid == MAVLINK_MSG_ID_SET_MODE)
            {
                qDebug() << "-----Recieved SET MODE-----";
                mavlink_set_mode_t recv_msg;
                mavlink_msg_set_mode_decode(&msg, &recv_msg);
                set_mode_msg = recv_msg;
                return msg.msgid;
            }

            //MAVLINK message #69
            else if(msg.msgid == MAVLINK_MSG_ID_MANUAL_CONTROL)
            {
                qDebug() << "-----Recieved Manual Control-----";
                mavlink_manual_control_t recv_msg;
                mavlink_msg_manual_control_decode(&msg, &recv_msg);
                manual_control_msg = recv_msg;
                return msg.msgid;
            }

        }
    }
}

mavlink_heartbeat_t MavlinkParser::get_msg_heartbeat()
{
    return heartbeat_msg;
}

mavlink_param_request_list_t MavlinkParser::get_msg_param_request_list()
{
    return param_request_list_msg;
}

mavlink_param_request_read_t MavlinkParser::get_msg_param_request_read()
{
    return param_request_read_msg;
}

mavlink_mission_request_list_t MavlinkParser::get_msg_mission_request_list()
{
    return mission_request_list_msg;
}

mavlink_command_long_t MavlinkParser::get_msg_command_long()
{
    return command_long_msg;
}

mavlink_command_ack_t MavlinkParser::get_msg_command_ack()
{
    return command_ack_msg;
}

mavlink_set_mode_t MavlinkParser::get_msg_set_mode()
{
    return set_mode_msg;
}

mavlink_manual_control_t MavlinkParser::get_msg_manual_control()
{
    return manual_control_msg;
}

uint8_t MavlinkParser::getMavMessageID(QByteArray data)
{
    parseMavPackets(data);
    return mav_msg_id;
}

uint8_t MavlinkParser::getMavSystemID(QByteArray data)
{
    parseMavPackets(data);
    return mav_msg_systemID;
}

uint8_t MavlinkParser::getMavCompID(QByteArray data)
{
    parseMavPackets(data);
    return mav_msg_compID;
}
