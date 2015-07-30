#include <pololu_session/client.h>
#include <base/connection.h>

namespace Pololu {
	struct Connection : Genode::Connection<Session>, Session_client
	{
		Connection()
			: Genode::Connection<Pololu::Session>(session("foo, ram_quota=4K")),
			Session_client(cap()) { }
	};
}
