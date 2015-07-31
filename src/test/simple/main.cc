/*
 * \brief  Simple Servo controller test
 * \author Daniel Krefft
 * \date   2015-07-30
 */


#include <base/printf.h>
#include <servo_controller_session/connection.h>

int main(int, char **)
{

    static Servo_Controller::Connection servo_controller;

    // Just for fun
    servo_controller.say_hello();

    // Testcases
    unsigned char channel = 0;
    unsigned short targets[] = {6000, 8000};
    while(1) {
	servo_controller.set_target(channel, targets[0]);
	// get position
	int position = servo_controller.get_position(channel);
	PDBG("Current position is %d.\n", position);

	// Speed
	PDBG("Speed Test begins");
	servo_controller.set_speed(channel, 1); // minimum speed
	servo_controller.set_target(channel, targets[1]);
	while(servo_controller.get_moving_state());
	PDBG("All targets arrived");
	position = servo_controller.get_position(channel);
	PDBG("Current position is %d.\n", position);
	servo_controller.set_speed(channel, 0); // unlimited speed
	servo_controller.set_target(channel, targets[0]);
	while(servo_controller.get_moving_state());
	PDBG("Speed Test ends");

	// Acceleration
	PDBG("Acc Test begins");
	servo_controller.set_acceleration(channel, 1); // minimum acc
	servo_controller.set_target(channel, targets[1]);
	while(servo_controller.get_moving_state());
	PDBG("All targets arrived");
	position = servo_controller.get_position(channel);
	PDBG("Current position is %d.\n", position);
	servo_controller.set_acceleration(channel, 0); // unlimited acc
	servo_controller.set_target(channel, targets[0]);
	while(servo_controller.get_moving_state());
	PDBG("Acc Test ends");
    }

    return 0;
}
