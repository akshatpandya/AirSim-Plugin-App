#ifndef RPCCLIENT_H
#define RPCCLIENT_H

#include "../AirSim/AirLib/include/common/common_utils/StrictMode.hpp"
STRICT_MODE_OFF
#ifndef RPCLIB_MSGPACK
#define RPCLIB_MSGPACK clmdep_msgpack
#endif // !RPCLIB_MSGPACK
#include "../AirSim/AirLib/deps/rpclib/include/rpc/rpc_error.h"
STRICT_MODE_ON


#include "../AirSim/AirLib/include/vehicles/multirotor/api/MultirotorRpcLibClient.hpp"
#include "joystick.h"
#include <QObject>
#include <QDebug>

using namespace msr::airlib;

class RPCclient : public QObject
{
    Q_OBJECT
public:
    RPCclient();
    void arm_disarm(bool arm);
    void takeOff(float time_out);
    void land(float time_out);
    void moveByRC(RCData& rc_data);
    void moveByRCYaw(RCData& rc_data);
    void disconnect();
    MultirotorState getMultirotorState();

private:
    MultirotorRpcLibClient client;
    joystick joystick_manager;
};

#endif // RPCCLIENT_H
