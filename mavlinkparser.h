#ifndef MAVLINKPARSER_H
#define MAVLINKPARSER_H

#include "common/mavlink.h"
#include <QTcpSocket>
#include <QByteArray>

class MavlinkParser
{
public:
    MavlinkParser();
    uint8_t parseMavPackets(QByteArray data);
    uint8_t getMavMessageID(QByteArray data);
    uint8_t getMavSystemID(QByteArray data);
    uint8_t getMavCompID(QByteArray data);

    mavlink_heartbeat_t get_msg_heartbeat();
    mavlink_param_request_list_t get_msg_param_request_list();
    mavlink_param_request_read_t get_msg_param_request_read();
    mavlink_mission_request_list_t get_msg_mission_request_list();
    mavlink_command_long_t get_msg_command_long();
    mavlink_command_ack_t get_msg_command_ack();
    mavlink_set_mode_t get_msg_set_mode();
    mavlink_manual_control_t get_msg_manual_control();

private:
    uint8_t mav_msg_id;
    uint8_t mav_msg_systemID;
    uint8_t mav_msg_compID;

    mavlink_heartbeat_t heartbeat_msg;
    mavlink_param_request_list_t param_request_list_msg;
    mavlink_param_request_read_t param_request_read_msg;
    mavlink_mission_request_list_t mission_request_list_msg;
    mavlink_command_long_t command_long_msg;
    mavlink_command_ack_t command_ack_msg;
    mavlink_set_mode_t set_mode_msg;
    mavlink_manual_control_t manual_control_msg;
};

#endif // MAVLINKPARSER_H
