#include <servo_controller_session/client.h>
#include <base/connection.h>

namespace Servo_Controller {
	struct Connection : Genode::Connection<Session>, Session_client
	{
		Connection()
			: Genode::Connection<Servo_Controller::Session>(session("foo, ram_quota=4K")),
			Session_client(cap()) { }
	};
}
