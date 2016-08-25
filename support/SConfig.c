/*
 * Copyright 2014, 2015 AASHTO/ITE/NEMA.
 * American Association of State Highway and Transportation Officials,
 * Institute of Transportation Engineers and
 * National Electrical Manufacturers Association.
 *  
 * This file is part of the Advanced Transportation Controller (ATC)
 * Application Programming Interface Reference Implementation (APIRI).
 *  
 * The APIRI is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 2.1 of the
 * License, or (at your option) any later version.
 *  
 * The APIRI is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *  
 * You should have received a copy of the GNU Lesser General Public
 * License along with the APIRI.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <fpui.h>

#define ESCkeycode 0x87

bool panel_change = false;

void signal_handler( int arg )
{
	panel_change = true;
}

int main(int argc, char **argv)
{
	int fd;
	struct sigaction act;

	memset(&act, 0, sizeof(act));
	act.sa_handler = signal_handler;
	act.sa_flags = 0;
	sigaction(SIGWINCH, &act, NULL);
printf("opening config window\n");	
	/* acquire the config window handle */
	while ((fd = fpui_open_config_window(O_RDWR|O_EXCL)) < 0) {
		printf("fpui_open_config_window: err=%d\n", errno);
		sleep(1);
	};
printf("config window opened\n");
	/* map <ESC> key sequence to keycode */
	fpui_set_keymap(fd, ESCkeycode, "\x1bOS");
printf("keycode mapped\n");
	/* clear display */
	fpui_clear(fd);
printf("cleared display\n");
	/* display test message */
	fpui_write_string_at(fd, "Test Config Utility", 5, 10);
printf("wrote message\n");
	panel_change = false;
	/* exit on escape key press */
	while (fpui_read_char(fd) != ESCkeycode) {
		if (panel_change == true) {
			panel_change = false;
			printf("panel change signal\n");
		}
	};
printf("received ESC keycode\n");
	/* release exclusive handle */
	fpui_close_config_window(fd);
	
	return 0;
}

