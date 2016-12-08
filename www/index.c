#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<dirent.h>
#include "sl_list.h"

#define _H printf
char *_glob_arg_str = NULL;
struct s_list_head *_glob_args = NULL;
struct KVP {char *key, *value;};

void init_args(char *argstr) 
{

	if(!argstr) return;

	_glob_arg_str = malloc(sizeof(char) * strlen(argstr));
	_glob_args = new_s_list_head();
	strcpy(_glob_arg_str, argstr);

	char *start = _glob_arg_str;
	int i, parse_started = 0, count = 0;
	struct KVP *kvp;

	while( *start )
	{
		if(!parse_started) {
			kvp = (struct KVP*)malloc(sizeof(struct KVP));
			kvp->key = start;
			parse_started = 1;
		}
		if(*start == '=') {
			*start = '\0';
			kvp->value = start + 1;
			s_list_add(_glob_args, kvp);
		}
		if(*start == '&')
			parse_started = 0;
		start++;
	}
}

char *getparam(char *argname)
{
	struct s_list_node *node;
	struct KVP *kvp;

	for(
		node = _glob_args->first;
		node;
		node = node->next
	) {
		kvp = (struct KVP*)node->data;
		if(!strcmp(kvp->key, argname)) {
			return kvp->value;
		}
	}
	return NULL;
}


int main(int argc, char *argv[])
{

	if(argc < 2) {
		fprintf(stderr, "No args passed");
		return 0;
	}

	init_args(argv[1]);

	char *imgtype = getparam("image");
	struct dirent *dir;
	DIR *d;

	d = opendir(".");

	_H(
		"<html>	\
			<head>	\
				<title>Home Page</title>	\
			</head>	\
			<body>	\
				<h1>Images of : %s</h1>",
		imgtype
	);

	if (d)
	{
		while ((dir = readdir(d)) != NULL)
		{
			if(!strstr(dir->d_name, imgtype))
				continue;
			_H("<img src=\"%s\" style=\"width: 200px\"/> <br /><br />", dir->d_name);
		}
		closedir(d);
	}

	_H(
			"</body>\
		</html>"
	);

	//close(1);
	return 0;
}