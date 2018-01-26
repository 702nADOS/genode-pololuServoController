#include <base/printf.h>
#include <base/sleep.h>
#include <base/rpc_server.h>

#include <servo_controller_session/servo_controller_session.h>

#include <root/component.h>
#include <cap_session/connection.h>
#include <terminal_session/connection.h>
#include <timer_session/connection.h>

namespace Servo_Controller {

    /*Pololu Maestro Servo Controller (PMSC)*/
    struct PMSC_component : Genode::Rpc_object<Session>
    {
    private:
	Terminal::Connection *_terminal;
	Timer::Connection *_timer;

    public:
	/**
	 * Constructor
	 */
	PMSC_component(Terminal::Connection *terminal, Timer::Connection *timer)
	    : _terminal(terminal),
	      _timer(timer) {}

	/********************
	 ** servo controller session **
	 ********************/

	void say_hello() {
	    PDBG("Ready ... Maestro."); }

	void set_target(unsigned char channel, unsigned short target) {
	    unsigned char command[]  = {0xAA, 0xC, 0x04, channel,
					target & 0x7F, target >> 7 & 0x7F};
	    
		if(_terminal->write(command, sizeof(command)) < sizeof(command))
	    {
		PDBG("error writing");
	    }
	}

	void set_speed(unsigned char channel, unsigned short speed) {
	    unsigned char command[] = {0xAA, 0xC, 0x07, channel,
				       speed & 0x7F, speed >> 7 & 0x7F };
	    if(_terminal->write(command, sizeof(command)) < sizeof(command))
	    {
		PDBG("error writing");
	    }
	}

	void set_acceleration(unsigned char channel, unsigned short acc) {
	    unsigned char command[] = {0xAA, 0xC, 0x09, channel,
				       acc & 0x7F, acc >> 7 & 0x7F };
	    if(_terminal->write(command, sizeof(command)) < sizeof(command))
	    {
		PDBG("error writing");
	    }
	}

	// untested
	void set_pwm(unsigned char channel, unsigned short on_time, unsigned short period) {
		unsigned char command[] = {0xAA, 0xC, 0x0A, channel,
					   on_time & 0x7F, on_time >> 7 & 0x7F,
					   period & 0x7F, period >> 7 & 0x7F };
		if(_terminal->write(command, sizeof(command)) < sizeof(command))
		{
		    PDBG("error writing");
		}
	}

	int get_position(unsigned char channel) {
	    unsigned char command[] = {0xAA, 0xC, 0x10, channel};
	    unsigned char response[2];
	    if(_terminal->write(command, sizeof(command)) < sizeof(command))
	    {
		PDBG("error writing");
		return -1;
	    }

	    // todo: sig_rec->wait_for_singnal()
	    _timer->msleep(1000);

	    if(_terminal->read(response, sizeof(response)) != 2)
	    {
		PDBG("error reading");
		return -1;
	    }

	    return response[0] + 256*response[1];
	}

	unsigned char get_moving_state(void) {
	    unsigned char command[] = {0xAA, 0xc, 0x13};
	    unsigned char response;
	    if(_terminal->write(command, sizeof(command)) < sizeof(command))
	    {
		PDBG("error writing");
		return -1;
	    }

	    // todo: sig_rec->wait_for_singnal()
	    _timer->msleep(1000);

	    if(_terminal->read(&response, 1) != 1)
	    {
		PDBG("error reading");
		return -1;
	    }
	    return response;
	}
    };


    class PMSC_root : public Genode::Root_component<PMSC_component>
    {
    private:
	Terminal::Connection *_terminal;
	Timer::Connection *_timer;

    protected:
	PMSC_component *_create_session(const char *args)
	    {
		PDBG("Creating Pololu session.");
		return new (md_alloc()) PMSC_component(_terminal, _timer);
	    }

    public:
	PMSC_root(Genode::Rpc_entrypoint *ep,
		  Genode::Allocator *allocator,
		  Terminal::Connection *terminal,
		  Timer::Connection *timer)
	    : Genode::Root_component<PMSC_component>(ep, allocator),
	    _terminal(terminal), _timer(timer)
	    {
		PDBG("Creating root component.");
	    }
    };
}

int main(void)
{
    using namespace Genode;
    using namespace Servo_Controller;

    /*
     * Open Terminal session
     */
    static Terminal::Connection terminal;
    static Timer::Connection timer;

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
    static Rpc_entrypoint ep(&cap, STACK_SIZE, "pmsc_ep");

    static PMSC_root pmsc_root(&ep, /* &sliced_heap */
			       env()->heap(),
			       &terminal,
			       &timer);
    /*
     * Announce services
     */
    env()->parent()->announce(ep.manage(&pmsc_root));

    /*
      !   * We are done with this and only act upon client requests now.
      !   */
    sleep_forever();
    return 0;
}
