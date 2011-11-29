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
