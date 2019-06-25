#include "joystick.h"
#include <QDebug>

joystick::joystick()
{
    restart = false;
    pitch = 0;
    roll = 0;
    yaw = 0;
}

void joystick::move(RCData rc_data, MultirotorRpcLibClient* client)
{
    joystick_packet.timestamp = rc_data.timestamp;
    joystick_packet.pitch = rc_data.pitch;
    joystick_packet.roll = rc_data.roll;
    joystick_packet.throttle = rc_data.throttle;
    joystick_packet.yaw = rc_data.yaw;
    joystick_packet.is_initialized = rc_data.is_initialized;
    joystick_packet.is_valid = rc_data.is_valid;
    rpc_client = client;
    restart = true;
    start();
}

void joystick::run()
{
    forever
    {
        restart = false;
        if(restart)
            continue;

        mutex.lock();
        MultirotorState uav_state = rpc_client->getMultirotorState();
        airsim.toEulerianAngle(uav_state.kinematics_estimated.pose.orientation, pitch, roll, yaw);

        float vy = joystick_packet.roll*cos(yaw) + joystick_packet.pitch*sin(yaw);
        float vx = joystick_packet.pitch*cos(yaw) - joystick_packet.roll*sin(yaw);

        if(abs(joystick_packet.yaw) < 0.5)
            rpc_client->moveByVelocityAsync(vx*15, vy*15, joystick_packet.throttle*5, 1, DrivetrainType::MaxDegreeOfFreedom);
        else
            rpc_client->rotateByYawRateAsync(joystick_packet.yaw*50,1);
        mutex.unlock();
        return;
    }

}
