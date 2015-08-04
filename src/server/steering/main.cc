#include <base/printf.h>
#include <base/sleep.h>
#include <base/rpc_server.h>

#include <servo_controller_session/connection.h>

#include <root/component.h>
#include <cap_session/connection.h>

namespace Steering {

    struct Steering_component : Genode::Rpc_object<Session>
    {
    private:
	Servo_Controller::Connection *_sc;

    public:
	/**
	 * Constructor
	 */
	Steering_component(Servo_Controller::Connection *sc)
	    : _sc(sc) {}

	/********************
	 ** steering session **
	 ********************/
	void set_steering_angle(float angle) {
	    unsigned short target = 0;
	    unsigned short low = 1250;
	    unsigned short high = 1750;
	    float us_step = (high-low) / angle;

	    target = (unsigned short)(angle * us_step * 4);

	    PDBG("Given angle %f -> target %d", angle, target);

	    // channel, target
	    _sc->set_target(0, target);
	}

    };


    class Steering_root : public Genode::Root_component<Steering_component>
    {
    private:
	Servo_Controller::Connection *_sc;
    protected:
	Steering_component *_create_session(const char *args)
	    {
		PDBG("Creating Steering session.");
		return new (md_alloc()) Steering_component(_sc);
	    }

    public:
	Steering_root(Genode::Rpc_entrypoint *ep,
		      Genode::Allocator *allocator,
		      Servo_Controller::Connection *sc)
	    : Genode::Root_component<Steering_component>(ep, allocator),
	    _sc(sc)
	    {
		PDBG("Creating root component.");
	    }
    };
}

int main(void)
{
    using namespace Genode;

    /*
     * Open Servo Controller session
     */
    static Servo_Controller::Connection sc;

    /*
     * Get a session for the parent's capability service, so that we
     * are able to create capabilities.
     */
    Cap_connection cap;

    /*
      !   * A sliced heap is used for allocating session objects - thereby we
      !   * can release objects separately.
      !   */
    // static Sliced_heap sliced_heap(env()->ram_session(),
    //				   env()->rm_session());

    /*
      !   * Create objects for use by the framework.
      !   *
      !   * An 'Rpc_entrypoint' is created to announce our service's root
      !   * capability to our parent, manage incoming session creation
      !   * requests, and dispatch the session interface. The incoming RPC
      !   * requests are dispatched via a dedicated thread. The 'STACK_SIZE'
      !   * argument defines the size of the thread's stack. The additional
      !   * string argument is the name of the entry point, used for
      !   * debugging purposes only.
      !   */
    enum { STACK_SIZE = 4096 };
    static Rpc_entrypoint ep(&cap, STACK_SIZE, "steering_ep");

    static Steering_root steering_root(&ep, /* &sliced_heap */
				       env()->heap(),
				       &sc);
    /*
     * Announce services
     */
    env()->parent()->announce(ep.manage(&steering_root));

    /*
      !   * We are done with this and only act upon client requests now.
      !   */
    sleep_forever();
    return 0;
}
