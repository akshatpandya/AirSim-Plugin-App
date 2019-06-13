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
#include <QObject>
#include <QDebug>

using namespace msr::airlib;

//basic_string("127.0.0.1",char,const T*)

class RPCclient : public QObject
{
    Q_OBJECT
public:
    RPCclient();
    void arm_disarm(bool arm);
    void takeOff(float time_out);
    void land(float time_out);
    void moveByRC(RCData& rc_data);
    void disconnect();
    MultirotorState getMultirotorState();

private:
    MultirotorRpcLibClient client;

};

#endif // RPCCLIENT_H
