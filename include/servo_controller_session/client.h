#include <pololu_session/pololu_session.h>
#include <base/rpc_client.h>
#include <base/printf.h>

namespace Pololu {
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

		int get_position(unsigned char channel)
		{
			PDBG("Get Position.");
			return call<Rpc_get_position>(channel);
		}
	};

}
