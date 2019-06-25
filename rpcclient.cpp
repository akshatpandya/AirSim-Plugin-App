#include "rpcclient.h"

RPCclient::RPCclient()
{
    while(!((uint8_t)client.getConnectionState() == 1))
    {
        qDebug() << "Connecting to AirSim....." << (uint8_t)client.getConnectionState();;
        client.confirmConnection();
    }

    while(!(client.isApiControlEnabled() == true))
    {
        qDebug() << "Enabling API control.....";
        client.enableApiControl(true);
    }

//    joystick_manager = new joystick(&client);
}

void RPCclient::arm_disarm(bool arm)
{
    if(arm == true)
        client.armDisarm(true);
    else
        client.armDisarm(false);
}

void RPCclient::takeOff(float time_out)
{
    client.takeoffAsync(time_out);
}

void RPCclient::moveByRC(RCData& rc_data)
{
//    client.moveByVelocityAsync(rc_data.pitch*5, rc_data.roll*5, rc_data.throttle*5, 0.5, DrivetrainType::MaxDegreeOfFreedom, yaw_mode);
//    client.rotateByYawRateAsync(rc_data.yaw*50, 0.5);
    joystick_manager.move(rc_data, &client);
}

void RPCclient::moveByRCYaw(RCData &rc_data)
{
    client.rotateByYawRateAsync(rc_data.yaw*50, 0.5);
}

void RPCclient::land(float time_out)
{
    client.landAsync(time_out);
}

void RPCclient::disconnect()
{
    client.reset();
}

MultirotorState RPCclient::getMultirotorState()
{
    MultirotorState uav_state;
    uav_state = client.getMultirotorState();
    return uav_state;
}
