#ifndef _PARSE_CONF_H
#define _PARSE_CONF_H

#include "common.h"

#define MAXLEN 80

#define THY_STR(parameter)   else if ( strcmp(name, #parameter) == 0 ) { strcpy(para_conf_s->parameter, value); }
#define THY_INT(parameter)   else if ( strcmp(name, #parameter) == 0 ) { para_conf_s->parameter = atoi(value);}


int parse_conf(void);
int init_para_conf(struct para_conf *para);
int check_para(struct para_conf *para, char *section, char *name, char *value);
int parse_config(char *path, struct para_conf *para);


#endif
