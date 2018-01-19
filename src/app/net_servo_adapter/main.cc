#include <base/env.h>
#include <base/printf.h>
#include <base/sleep.h>
#include <cap_session/connection.h>
#include <os/config.h>
#include <root/component.h>
#include <util/xml_node.h>
#include <timer_session/connection.h>
#include <base/rpc_server.h>

extern "C" {
#include <lwip/sockets.h>
}
#include <lwip/genode.h>
#include <nic/packet_allocator.h>

// #include <controller_session/client.h>
// #include <controller_session/connection.h>
// #include "../../mqtt/mqtt_entity.h"

#include "mqtt_entity.h"

// Servo channel configuration
// #define STEER_CHANNEL "6"
// #define BRAKE_LEFT_FRONT_CHANNEL "1"
// #define BRAKE_RIGHT_FRONT_CHANNEL "2"
// #define BRAKE_REAR_CHANNEL "0"

const unsigned int STEER_CHANNEL = 6;
const unsigned int BRAKE_LEFT_FRONT_CHANNEL = 1;
const unsigned int BRAKE_RIGHT_FRONT_CHANNEL = 2;
const unsigned int BRAKE_REAR_CHANNEL = 0;
const static int SERVO_UPPER_BOUND = 7500;
const static int SERVO_LOWER_BOUND = 4500;


enum IDs {
    STEER = 0, // [-1;1]
    BRAKE = 1, // [0;1]
    ACCEL = 2  // [0,1]
};

int transform_steer(double value) {
    if (value < -1 || value > 1) {
        PERR("Invalid steering angle - range is -1 to 1");
        return -1;
    }

    // Invert value as SpeedDreams thinks -1 is right
    value = -value;

    value = (value + 1)/2;
    return (SERVO_UPPER_BOUND - SERVO_LOWER_BOUND) * value + SERVO_LOWER_BOUND;
}

int transform_brake(double value) {
    if (value < 0 || value > 1) {
        PERR("Invalid target brake position - range is 0 to 1");
        return -1;
    }

    return (SERVO_UPPER_BOUND - SERVO_LOWER_BOUND) * value + SERVO_LOWER_BOUND;
}


int main(void)
{
    using namespace Genode;

    /*
     * Network configuration
     */
    Genode::Xml_node network = Genode::config()->xml_node().sub_node("network");
    enum { BUF_SIZE = Nic::Packet_allocator::DEFAULT_PACKET_SIZE * 128 };
    if (network.attribute_value<bool>("dhcp", true)) {
        PDBG("DHCP network...");
        if (lwip_nic_init(0,
                    0,
                    0,
                    BUF_SIZE,
                    BUF_SIZE)) {
            PERR("lwip init failed!");
            return 1;
        }
    } else {
        PDBG("Manual network...");
        char ip_addr[16] = {0};
        char subnet[16] = {0};
        char gateway[16] = {0};

        network.attribute("ip-address").value(ip_addr, sizeof(ip_addr));
        network.attribute("subnet-mask").value(subnet, sizeof(subnet));
        network.attribute("default-gateway").value(gateway, sizeof(gateway));

        if (lwip_nic_init(inet_addr(ip_addr),
                    inet_addr(subnet),
                    inet_addr(gateway),
                    BUF_SIZE,
                    BUF_SIZE)) {
            PERR("lwip init failed!");
            return 1;
        }
    }
    PDBG("Network configuration done, waiting for IP");
    Timer::Connection timer;
    timer.msleep(10000);

    /*
     * MQTT setup
     */
    Genode::Xml_node mosquitto = Genode::config()->xml_node().sub_node("mosquitto");

    char ip_addr[16] = {0};
    char port[5] = {0};
    char id[16] = {0};
    char topic[16] = {0};
    char subscribe[16] = {0};
    mosquitto.attribute("ip-address").value(ip_addr, sizeof(ip_addr));
    mosquitto.attribute("port").value(port, sizeof(port));
    mosquitto.attribute("id").value(id,sizeof(id));
    mosquitto.attribute("topic").value(topic,sizeof(topic));
    mosquitto.attribute("subscribe").value(subscribe,sizeof(subscribe));

    PDBG("Connecting to MQTT server");
    // Mqtt_Entity *mqtt_entity = new Mqtt_Entity("panda", "car-servo", ip_addr);
    Mqtt_Entity *mqtt_entity = new Mqtt_Entity(id, topic, ip_addr);
    mqtt_entity->my_subscribe(subscribe);
    //mqtt_entity->my_subscribe("car-control");

    // /*
    //  * Controller connection
    //  */
    // PDBG("Controller init");
    // Controller::Connection controller;

    /*
     * Handle mqtt messages
     */
    char recv_cmd[50];
    char servo_cmd[10];
    char *split, *cmd, *target;
    double value;
    int servoVal;
    while (true) {
        mqtt_entity->wait_msg();
        mqtt_entity->get_cmd(recv_cmd, sizeof(recv_cmd));

        split = strtok(recv_cmd, ",");
        if (!split) {
            continue;
        }
        cmd = split;

        target = strtok(NULL, ",");
        if (!target) {
            continue;
        }
        value = atof(target);

        switch (strtoul(cmd, NULL, 0)) {
            case STEER:
                servoVal = transform_steer(value);
                snprintf(servo_cmd, sizeof(servo_cmd), "%d,%d", STEER_CHANNEL, servoVal);
                mqtt_entity->send_message(servo_cmd);
                break;
            case BRAKE:
                servoVal = transform_brake(value);
                snprintf(servo_cmd, sizeof(servo_cmd), "%d,%d", BRAKE_LEFT_FRONT_CHANNEL, servoVal);
                mqtt_entity->send_message(servo_cmd);
                snprintf(servo_cmd, sizeof(servo_cmd), "%d,%d", BRAKE_RIGHT_FRONT_CHANNEL, servoVal);
                mqtt_entity->send_message(servo_cmd);
                snprintf(servo_cmd, sizeof(servo_cmd), "%d,%d", BRAKE_REAR_CHANNEL, servoVal);
                mqtt_entity->send_message(servo_cmd);
                break;
            case ACCEL:
                // currently not implemented
                break;
            default:
                servoVal = -1;
                break;
        }
    }

    sleep_forever();

    delete mqtt_entity;
    return 0;
}
