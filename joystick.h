#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <QThread>
#include <QMutex>
#include "../AirSim/AirLib/include/vehicles/multirotor/api/MultirotorRpcLibClient.hpp"

using namespace msr::airlib;

class joystick: public QThread
{
public:
    joystick();
    void move(RCData rc_data, MultirotorRpcLibClient* client);

protected:
    void run() override;

private:
    bool restart;
    RCData joystick_packet;
    QMutex mutex;
    MultirotorRpcLibClient* rpc_client;
    msr::airlib::VectorMathT<Vector3r, Quaternionr, float> airsim;
    float_t pitch, roll, yaw;
};

#endif // JOYSTICK_H
