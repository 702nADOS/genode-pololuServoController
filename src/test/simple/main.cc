/*
 * \brief  Terminal echo program
 * \author Norman Feske
 * \date   2009-10-16
 */

/*
 * Copyright (C) 2009-2013 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#include <base/printf.h>
#include <base/signal.h>
#include <terminal_session/connection.h>
#include <timer_session/connection.h>

using namespace Genode;

static const bool verbose = false;

int maestroGetPosition(Terminal::Connection *term, Signal_receiver *sig_rec, unsigned char channel)
{
	unsigned char command[] = {0xAA, 0xC, 0x10, channel};
  	char response[2];
	if(term->write(command, sizeof(command)) < sizeof(command))
	{
		PDBG("error writing");
		return -1;
	}

	sig_rec->wait_for_signal();
	if(term->read(response, sizeof(response)) != 2)
	{
		PDBG("error reading");
		return -1;
	}
	return response[0] + 256*response[1];
}

int maestroSetTarget(Terminal::Connection *term, unsigned char channel, unsigned short target)
{
	unsigned char command[] = {0xAA, 0xC, 0x04, channel, target & 0x7F, target >> 7 & 0x7F};
	if(term->write(command, sizeof(command)) < sizeof(command))
	{
		PDBG("error writing");
		return -1;
	}
	return 0;
}

int maestroGetMovingState(Terminal::Connection *term, Signal_receiver *sig_rec)
{
	unsigned char command[] = {0xAA, 0xC, 0x13};
	char response[1];
	if(term->write(command, sizeof(command)) < sizeof(command))
	{
		PDBG("error writing");
		return -1;
	}

	sig_rec->wait_for_signal();
	if(term->read(response, sizeof(response)) != 1)
	{
		PDBG("error reading");
		return -1;
	}

	return response[0] == 0x00 ? 0 : 1;
}

int main(int, char **)
{
	static Terminal::Connection terminal;
	static Timer::Connection timer;

	enum { READ_BUFFER_SIZE = 100 };
	static char read_buffer[READ_BUFFER_SIZE];
	int channel = 0;
	int target = 0;

	Signal_receiver sig_rec;
	Signal_context  sig_ctx;

	terminal.read_avail_sigh(sig_rec.manage(&sig_ctx));

	int position = maestroGetPosition(&terminal, &sig_rec, channel);
	PDBG("Current position is %d.\n", position);

for(;;){
	target = 8000;
	//PDBG("Setting target to %d (%d us).\n", target, target/4);
	maestroSetTarget(&terminal, channel, target);
	timer.msleep(1000);
	//while(maestroGetMovingState(&terminal, &sig_rec));
/*	while(position < target) {
		position = maestroGetPosition(&terminal, &sig_rec, channel);
		PDBG("Current position is %d.\n", position);
	}*/

	target = 3968;
	//PDBG("Setting target to %d (%d us).\n", target, target/4);
	maestroSetTarget(&terminal, channel, target);
	timer.msleep(1000);
	//while(maestroGetMovingState(&terminal, &sig_rec));

/*	while(position > target) {
		position = maestroGetPosition(&terminal, &sig_rec, channel);
		PDBG("Current position is %d.\n", position);
	}	
*/
	target = 6000;
	//PDBG("Setting target to %d (%d us).\n", target, target/4);
	maestroSetTarget(&terminal, channel, target);
	timer.msleep(1000);
	//while(maestroGetMovingState(&terminal, &sig_rec));
/*	while(position < target) {
		position = maestroGetPosition(&terminal, &sig_rec, channel);
		PDBG("Current position is %d.\n", position);
	}
*/
}

/*
	static const char *intro_text =
		"--- Terminal echo test started - now you can type characters to be echoed. ---\r\n";
	terminal.write(intro_text, strlen(intro_text) + 1);

	for (;;) {

		sig_rec.wait_for_signal();

		int num_bytes = terminal.read(read_buffer, sizeof(read_buffer));

		if (verbose && (num_bytes > 0))
			PDBG("got %d bytes", num_bytes);

		for (int i = 0; i < num_bytes; i++) {
			if (read_buffer[i] == '\r') {
				terminal.write("\n", 1);
			}

			terminal.write(&read_buffer[i], 1);
		}
	}
*/

	return 0;
}
