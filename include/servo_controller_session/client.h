#include <servo_controller_session/servo_controller_session.h>
#include <base/rpc_client.h>
#include <base/printf.h>

namespace  Servo_Controller {
	struct Session_client : Genode::Rpc_client<Session>
	{
		Session_client(Genode::Capability<Session> cap)
			: Genode::Rpc_client<Session>(cap) { }

		void say_hello()
		{
			PDBG("Saying Hello.");
			call<Rpc_say_hello>();
		}

		void set_target(unsigned char channel, unsigned short target)
		{
			PDBG("Set Target.");
			call<Rpc_set_target>(channel, target);
		}

		void set_speed(unsigned char channel, unsigned short speed)
		{
			PDBG("Set Speed.");
			call<Rpc_set_speed>(channel, speed);
		}

		void set_acceleration(unsigned char channel, unsigned short acc)
		{
			PDBG("Set Acceleration.");
			call<Rpc_set_acceleration>(channel, acc);
		}

		void set_pwm(unsigned char channel, unsigned short on_time,
			     unsigned short period)
		{
			PDBG("Set PWM.");
			call<Rpc_set_pwm>(channel, on_time, period);
		}

		int get_position(unsigned char channel)
		{
			PDBG("Get Position.");
			return call<Rpc_get_position>(channel);
		}

		unsigned char get_moving_state(void)
		{
			PDBG("Get Moving State.");
			return call<Rpc_get_moving_state>();
		}
	};

}
