/***************************************************************************
 *   This file is part of project cliofetion_plus                          *
 *   Copyright (C) 2011 kqdmqx <kqdmqx@gmail.com>                          *
 *   Copyright (C) 2011 glace <glacebai@gmail.com>                         * 
 *   Copyright (C) 2011 DoDo                                               *
 *   Copyright (C) 2011 lucifer                                            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include "cliofetion_plus_func.h"

struct globalArgs_t {
/* option inputed flags */
    int function_inputed;
    int user_number_inputed;
    int password_inputed;
    int target_number_inputed;
    int number_type_inputed;
    int msg_inputed;

/* option arguments */
    const char *function;
    const char *user_number;
    const char *password;
    const char *target_number;
    NumberType number_type;
    const char *msg;
    const char *desc;
    const char *localname;
    const char *groupname;
    int group;
    const char *input_file;
} globalArgs;

static const char *optString = "f:u:p:t:T:d:g:l:m:i:h?";

static const struct option longOpts[] = {
    { "function", required_argument, NULL, 'f' },
    { "user_number", required_argument, NULL, 'u' },
    { "password", required_argument, NULL, 'p' },
    { "target_number", required_argument, NULL, 't' },
    { "number_type", required_argument, NULL, 'T'},
    { "msg",required_argument,NULL,'m'},
    { "desc", required_argument, NULL, 'd'},
    { "group", required_argument, NULL, 'g'},
    { "localname", required_argument, NULL, 'l'},
    { "input_file", required_argument, NULL, 'i'},
    { "help", no_argument, NULL, 'h' },
    { NULL, no_argument, NULL, 0 }
};

/* Display program usage, and exit.
 */
void display_usage( void )
{
	/* ... */
    printf("Cliofetion-plus\t Copyright:kqdmqx\n");
    printf("Generic Options\n");
    printf("-h\thelp\tShow this message.\n");
    printf("-f\tfunction\tselect a function\n");
    printf("-u\tuser_number\tspecify your mobile number or fetion number to login\n");
    printf("-p\tpassword\tspecify your password\n");
    printf("-t\ttarget_number\tspecify the number you want to send a message to or to add as a new contact\n");
    printf("-T\tnumber_type\tspecify the target number's type\n");
    printf("-d\tdesc\tdescribe yourself when add a new contact\n");
    printf("-l\tlocalname\tnew contact's localname\n");
    printf("-g\tgroup\tnew contact's group\n");
    printf("-i\tinput_file\tinput by this file\n");
	exit(1);
}

void initialize_global_args() {
    globalArgs.function_inputed = 0;
    globalArgs.function = NULL;
    globalArgs.user_number_inputed = 0;
    globalArgs.user_number = NULL;
    globalArgs.password_inputed = 0;
    globalArgs.password = NULL;

    globalArgs.target_number_inputed = 0;
    globalArgs.target_number = NULL;
    globalArgs.msg_inputed = 0;
    globalArgs.msg = NULL;

}



int main( int argc, char *argv[] )
{
	int opt = 0;
	int longIndex = 0;
	
	/* Initialize globalArgs before we get to work. */
    initialize_global_args();

    /* Process the arguments with getopt_long(), then 
	 * populate globalArgs. 
	 */
	opt = getopt_long( argc, argv, optString, longOpts, &longIndex );
	while( opt != -1 ) {
		switch( opt ) {
			case 'f':
				globalArgs.function_inputed = 1;
                globalArgs.function = optarg;
				break;
				
			case 'u':
				globalArgs.user_number_inputed = 1;
                globalArgs.user_number = optarg;
				break;
				
			case 'p':
				globalArgs.password_inputed = 1;
                globalArgs.password = optarg;
				break;
				
			case 't':
				globalArgs.target_number_inputed = 1;
                globalArgs.target_number = optarg;
				break;

            case 'T':
                globalArgs.number_type_inputed = 1;
                if(strcmp(optarg,"fetion") == 0) {
                    globalArgs.number_type = FETION_NO;
                } else if(strcmp(optarg,"mobile") == 0) {
                    globalArgs.number_type = MOBILE_NO;
                } else {
                    globalArgs.number_type_inputed = 0;
                    fprintf(stderr,"Wrong parameter:targetNumberType must be fetion or mobile\n");
                    exit(1);
                }
			    break;

            case 'm':
                globalArgs.msg_inputed = 1;
                globalArgs.msg = optarg;
                break;

			case 'h':	/* fall-through is intentional */
			case '?':
				display_usage();
				break;

			case 0:		/* long option without a short arg */
				break;
				
			default:
				break;
		}
		
		opt = getopt_long( argc, argv, optString, longOpts, &longIndex );
	}

    if(!globalArgs.user_number_inputed || !globalArgs.password_inputed || !globalArgs.function_inputed) {
        fprintf(stderr,"miss parameter:user_number, password or function\n");
        display_usage();
    }
    if( !globalArgs.target_number_inputed || !globalArgs.number_type_inputed || !globalArgs.msg_inputed) {
        fprintf(stderr,"miss parameter:targetnumber, targetnumbertype or message\n");
        display_usage();
    }

    User *user;

    user = fx_login(globalArgs.user_number, globalArgs.password);
    if(!user)
	    return 1;

    if(fx_send_message(user,globalArgs.user_number, globalArgs.target_number, globalArgs.msg, globalArgs.number_type))
        return 1;

    fx_logout(user);

	
	return 0;
}
