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

#include <openfetion.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
#define BUFLEN 1024

extern User* fx_login(const char *mobileno, const char *password);

extern int fx_logout(User *user);

extern int fx_check_buddy(User *user, const char* checkno,NumberType notype);

extern int fx_add_buddy(User *user, NumberType notype,const char *no, const char *localname, const char *desc);

extern int fx_send_message(User *user, const char *mobileno, const char *receiveno, const char *message, NumberType notype);

extern int fx_print_buddylist(User *user);
