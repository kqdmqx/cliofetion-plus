/***************************************************************************
 *   Copyright (C) 2010 by lwp                                             *
 *   levin108@gmail.com                                                    *
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
 
#include "clifetion_plus_func.h" 
#define BUFLEN 1024
 
int fx_login(User *user, const char *mobileno, const char *password)
{
	Config           *config;
	FetionConnection *tcp;
	FetionSip        *sip;
	char             *res;
	char             *nonce;
	char             *key;
	char             *aeskey;
	char             *response;
	int               local_group_count;
	int               local_buddy_count;
	int               group_count;
	int               buddy_count;
	int               ret;
 
	/* construct a user object */
 	user = fetion_user_new(mobileno, password);
	/* construct a config object */
	config = fetion_config_new();
	/* attach config to user */
	fetion_user_set_config(user, config);
 
	/* start ssi authencation,result string needs to be freed after use */
	res = ssi_auth_action(user);
	/* parse the ssi authencation result,if success,user's sipuri and userid
	 * are stored in user object,orelse user->loginStatus was marked failed */
	parse_ssi_auth_response(res, user);
	free(res);
 
	/* whether needs to input a confirm code,or login failed
	 * for other reason like password error */
	if(USER_AUTH_NEED_CONFIRM(user) || USER_AUTH_ERROR(user)) {
		debug_error("authencation failed");
		return 1;
	}
 
	/* initialize configuration for current user */
	if(fetion_user_init_config(user) == -1) {
		debug_error("initialize configuration");
		return 1;
	}
 
	if(fetion_config_download_configuration(user) == -1) {
		debug_error("download configuration");
		return 1;
	}
 
	/* set user's login state to be hidden */
	fetion_user_set_st(user, P_HIDDEN);
 
	/* load user information and contact list information from local host */
	fetion_user_load(user);
	fetion_contact_load(user, &local_group_count, &local_buddy_count);
 
	/* construct a tcp object and connect to the sipc proxy server */
	tcp = tcp_connection_new();
	if((ret = tcp_connection_connect(tcp, config->sipcProxyIP, config->sipcProxyPort)) == -1) {
		debug_error("connect sipc server %s:%d\n", config->sipcProxyIP, config->sipcProxyPort);
		return 1;
	}
 
	/* construct a sip object with the tcp object and attach it to user object */
	sip = fetion_sip_new(tcp, user->sId);
	fetion_user_set_sip(user, sip);
 
	/* register to sipc server */
	if(!(res = sipc_reg_action(user))) {
		debug_error("register to sipc server");
		return 1;
	}
 
	parse_sipc_reg_response(res, &nonce, &key);
	free(res);
	aeskey = generate_aes_key();
 
	response = generate_response(nonce, user->userId, user->password, key, aeskey);
	free(nonce);
	free(key);
	free(aeskey);
 
	/* sipc authencation,you can printf res to see what you received */
	if(!(res = sipc_aut_action(user, response))) {
		debug_error("sipc authencation");
		return 1;
	}
 
	if(parse_sipc_auth_response(res, user, &group_count, &buddy_count) == -1) {
		debug_error("authencation failed");
		return 1;
	}
 
	free(res);
	free(response);
 
	if(USER_AUTH_ERROR(user) || USER_AUTH_NEED_CONFIRM(user)) {
		debug_error("login failed");
		return 1;
	}
 
	/* save the user information and contact list information back to the local database */
	fetion_user_save(user);
	fetion_contact_save(user);
 
	/* these... fuck the fetion protocol */
	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	char buf[1024];
	if(setsockopt(user->sip->tcp->socketfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) == -1) {
		debug_error("settimeout");
		return 1;
	}
	tcp_connection_recv(user->sip->tcp, buf, sizeof(buf));
 
	return 0;
}

void fx_logout(User *user)
{
    fetion_free_user(user);
    user = NULL;
}


int fx_send_message(User *user, const char *mobileno, const char *receiveno, const char *message, NumberType notype)
{
	Conversation *conv;
	Contact      *contact;
	Contact      *contact_cur;
	Contact      *target_contact = NULL;
	int           daycount;
	int           monthcount;
 
	/* send this message to yourself */
	if(*receiveno == '\0' || strcmp(receiveno, mobileno) == 0 || strcmp(receiveno, fetion_sip_get_sid_by_sipuri(user->sipuri)) == 0 ) {
		/* construct a conversation object with the sipuri to set NULL
		 * to send a message to yourself  */
		conv = fetion_conversation_new(user, NULL, NULL);
		if(fetion_conversation_send_sms_to_myself_with_reply(conv, message) == -1) {
			debug_info("send message \"%s\" to %s", message, user->mobileno);
			return 1;
		}
	}else{
		/* get the contact detail information by mobile number,
		 * note that the result doesn't contain sipuri */
		contact = fetion_contact_get_contact_info_by_no(user, receiveno, notype);
        if(!contact){
			debug_error("get contact information of %s", receiveno);
			return 1;
		}
 
		/* find the sipuri of the target user */
		foreach_contactlist(user->contactList, contact_cur) {
			if(strcmp(contact_cur->userId, contact->userId) == 0) {
				target_contact = contact_cur;
				break;
			}
		}
 
		if(!target_contact) {
			debug_error("sorry,maybe %s isn't in your contact list");
			return 1;
		}
 
		/* do what the function name says */
		conv = fetion_conversation_new(user, target_contact->sipuri, NULL);
		if(fetion_conversation_send_sms_to_phone_with_reply(conv, message, &daycount, &monthcount) == -1) {
			debug_error("send sms to %s", receiveno);
			return 1;
		
        }else{
			debug_info("successfully send sms to %s\nyou have sent %d messages today, %d messages this monthcount",
					receiveno, daycount, monthcount);
			return 0;
		}
	}
	return 0;
}
 

int fx_add_buddy(User *user, NumberType notype,const char *no, const char *localname, const char *desc)
{
	int buddylist = 0; //ungrouped
	Contact* contact = NULL;
	Config* config = NULL;
	int ret = 0;

	config = user->config;
    	FxList *phraselist = fetion_config_get_phrase(config);

	if(notype == FETION_NO)
	{
		if(strcmp(no , user->sId) == 0)
		{
			printf(">You can not add yourself as a friend");
			return 1;
		}
	}
	else
	{
		if(strcmp(no , user->mobileno) == 0)
		{
			printf("You can not add yourself as a friend");
			return 1;
		}
	}
	if(*no == '\0')
		return 1;

	contact = fetion_contact_add_buddy(user, no, notype, buddylist, localname, desc, 0, &ret);

	switch(ret)
	{
		case BUDDY_USER_EXIST :
			printf("The contact you want to add has already been in your contact list, please don't add him(her) repeatedly!");
			return 1;
		case BUDDY_SAME_USER_DAILY_LIMIT :
			printf("You have reached the daily limit of adding buddies, please retry later");
			return 1;
		case BUDDY_BAD_REQUEST :
			printf("Add buddy failed. Unknown reason!");
			return 1;
		default:
			break;
	}
    return 0;
}

int fx_check_buddy(User *user, const char* checkno,NumberType notype) {
    /*check the target number in your contact list or not */
    Contact * contact_cur;
    Contact * contact = NULL;
    Contact * target_contact = NULL;
	contact = fetion_contact_get_contact_info_by_no(user, checkno, notype);
    if(!contact){
		debug_error("get contact information of %s", checkno);
		return 1;
    }
 
	/* find the sipuri of the target user */
	foreach_contactlist(user->contactList, contact_cur) {
		if(strcmp(contact_cur->userId, contact->userId) == 0) {
			target_contact = contact_cur;
			break;
		}
	}

	if(!target_contact) {
		debug_error("%s isn't in your contact list",checkno);
		return 1;
	}

	debug_info("%s is in your contact list",checkno);
    return 0; 
}

void fx_print_contact_list(User *user) {
	Contact *contact_cur;
    char * sip;
   	foreach_contactlist(user->contactList, contact_cur) {
        sip = fetion_sip_get_sid_by_sipuri(contact_cur->sipuri);
        printf("%s %s\n",contact_cur->userId,sip);
        //printf("%s %s : %s : %s : %s\n", contact_ptr->userId, contact_ptr->localname, contact_ptr->nickname, contact_ptr->mobileno, contact_ptr->sipuri);
	}
}
