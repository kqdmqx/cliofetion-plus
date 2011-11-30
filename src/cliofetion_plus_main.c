/***************************************************************************
 *   This file is part of  Cliofetion+ Project                             *
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
#include "xmlitem.h"

typedef enum {WRONG,SEND,ADD_BUDDY,CHECK_BUDDY} function_t;

struct globalArgs_t {
/* option inputed flags */
    int function_inputed;
    int user_number_inputed;
    int password_inputed;
    int target_number_inputed;
    int number_type_inputed;
    int msg_inputed;
    int localname_inputed;
    int desc_inputed;
    int input_file_inputed;
    int create_success_file;

/* option arguments */
//    const char *function;
    function_t function;
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

static const char *optString = "f:u:p:t:T:d:g:l:m:i:rh?";

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
    { "result_file" ,no_argument, NULL, 'r'},
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
    printf("-h\thelp\t\tShow this message.\n");
    printf("-f\tfunction\tselect a function\n");
    printf("-u\tuser_number\tspecify your mobile number or fetion number to login\n");
    printf("-p\tpassword\tspecify your password\n");
    printf("-t\ttarget_number\tspecify the number you want to send a message to or to add as a new contact\n");
    printf("-T\tnumber_type\tspecify the target number's type\n");
    printf("-d\tdesc\t\tdescribe yourself when add a new contact\n");
    printf("-l\tlocalname\tnew contact's localname\n");
    printf("-g\tgroup\tnew contact's group\n");
    printf("-i\tinput_file\tinput by this file\n");
    printf("-r\tresult_file\tcreate a fetion.success in current dir while successly msg is sent");
	exit(1);
}

void initialize_global_args() {
    globalArgs.function_inputed = 0;
    globalArgs.function = WRONG;
    globalArgs.user_number_inputed = 0;
    globalArgs.user_number = NULL;
    globalArgs.password_inputed = 0;
    globalArgs.password = NULL;

    globalArgs.target_number_inputed = 0;
    globalArgs.target_number = NULL;
    globalArgs.number_type_inputed = 0;
    globalArgs.msg_inputed = 0;
    globalArgs.msg = NULL;

    globalArgs.localname_inputed = 0;
    globalArgs.localname = NULL;
    globalArgs.desc = NULL;

    globalArgs.create_success_file = 0;
}

void set_global_function(char * func) {
	if(strcmp("send",func) == 0) {
        globalArgs.function = SEND;
    } else if(strcmp("addbuddy",func) == 0) {
        globalArgs.function = ADD_BUDDY;
    } else if(strcmp("checkbuddy",func) == 0) {
        globalArgs.function = CHECK_BUDDY;
    }
}

void set_global_number_type(char *notype) {
    if(strcmp(notype,"fetion") == 0) {
        globalArgs.number_type = FETION_NO;
    } else if(strcmp(notype,"mobile") == 0) {
        globalArgs.number_type = MOBILE_NO;
    } else {
        globalArgs.number_type_inputed = 0;
        debug_error("targetNumberType must be fetion or mobile");
        exit(1);
    }
}

void parse_cmd_opt(int argc, char *argv[]) {
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
                set_global_function(optarg);
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
                set_global_number_type(optarg);
			    break;

            case 'm':
                globalArgs.msg_inputed = 1;
                globalArgs.msg = optarg;
                break;

            case 'l':
                globalArgs.localname_inputed = 1;
                globalArgs.localname = optarg;
                break;

            case 'd':
                globalArgs.desc_inputed = 1;
                globalArgs.desc = optarg;
                break;

            case 'i':
                globalArgs.input_file_inputed = 1;
                globalArgs.input_file = optarg;
                break;

            case 'r':
                globalArgs.create_success_file = 1;
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

}

int can_longin() {
    if(!globalArgs.user_number_inputed){
        debug_error("miss user number");
        return 0;
    
    }
    if(!globalArgs.password_inputed){
        debug_error("miss password");
        return 0;
    }
    return 1;
}

int can_send_by_file() {
    if(!globalArgs.target_number_inputed) {
        debug_error("miss target number");
        return 0;
    }
    if(!globalArgs.number_type_inputed) {
        debug_error("miss number type");
        return 0;
    }
    if(!globalArgs.input_file_inputed) {
        debug_info("no input file");
        return 0;
    }
    return 1;
}

int can_send() {
    if(!globalArgs.target_number_inputed) {
        debug_error("miss target number");
        return 0;
    }
    if(!globalArgs.number_type_inputed) {
        debug_error("miss number type");
        return 0;
    }
    if(!globalArgs.msg_inputed) {
        debug_info("no message from cmd");
        return 0;
    }
    return 1;
}

int can_add() {
    if(!globalArgs.target_number_inputed) {
        debug_error("miss target number");
        return 0;
    }
    if(!globalArgs.number_type_inputed) {
        debug_error("miss number type");
        return 0;
    }
    if(!globalArgs.desc_inputed) {
        debug_error("miss description");
        return 0;
    }
    return 1;
}

int can_check() {
    if(!globalArgs.target_number_inputed) {
        debug_error("miss target number");
        return 0;
    }
    if(!globalArgs.number_type_inputed) {
        debug_error("miss number type");
        return 0;
    }
    return 1;
}

int send_msg_in_file(User *user,int *msg_count) {
    importXmlItemFileForAlarm(globalArgs.input_file, xmlItemAlarmList, &xmlItemListAlarmTotal);
    
    int result = 0;
    int i;
    for (i=1;i<=xmlItemListAlarmTotal;i++) {
        if(fx_send_message(user,globalArgs.user_number, globalArgs.target_number, xmlItemAlarmList[i].message, globalArgs.number_type)){
            result = 1;
            break;
        } else {
            (*msg_count)++;
        }
    }

    return result;
}

int create_success_file() {
    FILE *fp;
    if((fp = fopen("./fetion.success","wb")) == NULL) {
        return 1;
    }

    fclose(fp);
    return 0;
}

int do_chosen_function(){
    int result = 0;
    int success_flag = 0;
    int msg_count = 0;

    if(!globalArgs.function_inputed) {
        display_usage();
        exit(1);
    }

    User *user;
    if(!can_longin()) {
        debug_error("cant login");
        exit(1);
    }

    user = fx_login(globalArgs.user_number, globalArgs.password);     
    if(!user)
        return 1;

    switch(globalArgs.function) {
        case SEND:
            if(can_send_by_file()) {
                result = send_msg_in_file(user,&msg_count);
                if(!result)
                    success_flag++;
            }
            if(can_send()) {
                success_flag--;
                result = fx_send_message(user,globalArgs.user_number, globalArgs.target_number, globalArgs.msg, globalArgs.number_type);
                if(!result){
                    success_flag++;
                    msg_count++;
                }
            }
            if( globalArgs.create_success_file && success_flag) {
                create_success_file();
            }
            if(msg_count == 0) {
                debug_error("no message has been sent");
            }
            break;
        case ADD_BUDDY:
            if(!can_add()) {
                debug_error("cant add");
                result = 1;
                break;
            }
            if(!globalArgs.localname_inputed) {
                result = fx_add_buddy(user,globalArgs.number_type,globalArgs.target_number,"",globalArgs.desc);
            } else {
                result = fx_add_buddy(user,globalArgs.number_type,globalArgs.target_number,globalArgs.localname,globalArgs.desc);
            }
            break;
        case CHECK_BUDDY:
            if(!can_check()) {
                debug_error("cant check");
                result = 1;
                break;
            }
            result = fx_check_buddy(user,globalArgs.target_number,globalArgs.number_type); 
            break;
        default:
            debug_error("Unknown function");
    }

    fx_logout(user);

    return result;
}


int main( int argc, char *argv[] ) {
    parse_cmd_opt(argc,argv);
    if(do_chosen_function())
        return 1;
	
	return 0;
}
