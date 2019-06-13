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
//    client.moveByRC(rc_data);
    YawMode yaw_mode;
    yaw_mode.is_rate = true;
//    if(rc_data.pitch > 0.9)
//        client.moveByVelocityAsync(5, 0, 0, 1, DrivetrainType::MaxDegreeOfFreedom);
//    else if(rc_data.pitch < -0.9)
//        client.moveByVelocityAsync(-5, 0, 0, 1, DrivetrainType::MaxDegreeOfFreedom);
//    else if(abs(rc_data.pitch) <= 0.9)
//        client.moveByVelocityAsync(0, 0, 0, 1, DrivetrainType::MaxDegreeOfFreedom);

//    if(rc_data.roll > 0.9)
//        client.moveByVelocityAsync(0, 5, 0, 1, DrivetrainType::MaxDegreeOfFreedom);
//    else if(rc_data.roll < -0.9)
//        client.moveByVelocityAsync(0, -5, 0, 1, DrivetrainType::MaxDegreeOfFreedom);
//    else if(abs(rc_data.roll <= 0.9))
//        client.moveByVelocityAsync(0, 0, 0, 1, DrivetrainType::MaxDegreeOfFreedom);

//    if(rc_data.throttle > 0.9)
//        client.moveByVelocityAsync(0, 0, 5, 1, DrivetrainType::MaxDegreeOfFreedom);
//    else if(rc_data.throttle < -0.9)
//        client.moveByVelocityAsync(0, 0, -5, 1, DrivetrainType::MaxDegreeOfFreedom);
//    else if(abs(rc_data.throttle <= 0.9))
//        client.moveByVelocityAsync(0, 0, 0, 1, DrivetrainType::MaxDegreeOfFreedom);

//    if(rc_data.yaw > 0.9)
//        client.rotateByYawRateAsync(rc_data.yaw*10, 1);
//    else if(rc_data.yaw < -0.9)
//        client.rotateByYawRateAsync(-rc_data.yaw*10, 1);
//    else if(abs(rc_data.yaw <= 0.9))
//        client.rotateByYawRateAsync(0, 1);

    client.moveByVelocityAsync(rc_data.pitch*5, 0, 0, 1, DrivetrainType::MaxDegreeOfFreedom);
    client.moveByVelocityAsync(0, rc_data.roll*5, 0, 1, DrivetrainType::MaxDegreeOfFreedom);
    client.moveByVelocityAsync(0, 0, rc_data.throttle*5, 1, DrivetrainType::MaxDegreeOfFreedom);
    client.rotateByYawRateAsync(rc_data.yaw*50, 1);
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
