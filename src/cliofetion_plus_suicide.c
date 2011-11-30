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

#include "cliofetion_plus_suicide.h"
unsigned globle_time_limit;

void fx_sui_set_time_limit(unsigned time_limit) {
    globle_time_limit = time_limit;
}

void *suicide(void *arg){
    sleep(globle_time_limit);
    debug_error("Time up");
    exit(0);
}

void fx_sui_start_killer_thread() {
    int res;
    pthread_t a_thread;

    res = pthread_create(&a_thread, NULL, suicide, NULL);
    
    if (0 != res) {
        debug_error("Killer thread creation failed");
        exit(1);
    }
}
