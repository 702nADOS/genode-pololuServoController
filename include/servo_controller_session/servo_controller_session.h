#include <session/session.h>
#include <base/rpc.h>

namespace Servo_Controller {

	struct Session : public Genode::Session
	{
		static const char *service_name() {return "Servo Controller"; }

		virtual void say_hello() = 0;
		virtual void set_target(unsigned char channel, unsigned short target) = 0;
		virtual void set_speed(unsigned char channel, unsigned short speed) = 0;
		virtual void set_acceleration(unsigned char channel, unsigned short acc) = 0;
		// untested
		virtual void set_pwm(unsigned char channel, unsigned short on_time,
				     unsigned short period) = 0;
		virtual int get_position(unsigned char channel) = 0;
		virtual unsigned char get_moving_state(void) = 0;

		/* return-type, fn-name, parameters */
		GENODE_RPC(Rpc_say_hello, void, say_hello);
		GENODE_RPC(Rpc_set_target, void, set_target, unsigned char, unsigned short);
		GENODE_RPC(Rpc_set_speed, void, set_speed, unsigned char, unsigned short);
		GENODE_RPC(Rpc_set_acceleration, void, set_acceleration,
			   unsigned char, unsigned short);
		GENODE_RPC(Rpc_set_pwm, void, set_pwm, unsigned char, unsigned short,
			   unsigned short);
		GENODE_RPC(Rpc_get_position, int, get_position, unsigned char);
		GENODE_RPC(Rpc_get_moving_state, unsigned char, get_moving_state);
		GENODE_RPC_INTERFACE(Rpc_say_hello, Rpc_set_target,
				     Rpc_set_speed, Rpc_set_acceleration,
				     Rpc_set_pwm, Rpc_get_position,
				     Rpc_get_moving_state);
	};
}
