/*
 * Hash version 1.0
 *
 * hash_f() = N(base 26) % -1
 *
 */

#define   CONNECTION            568816507               //Connection
#define   DATE                  1241361                 //Date
#define   PRAGMA                989174615               //Pragma
#define   TRAILER               1184433986              //Trailer
#define   TRANSFER_ENCODING     694731304               //Transfer-Encoding
#define   UPGRADE               1472479575              //Upgrade
#define   VIA                   60099                   //Via
#define   WARNING               1917052589              //Warning

#define   OPTIONS               3919744456              //OPTIONS
#define   GET                   49874                   //GET
#define   HEAD                  1313874                 //HEAD
#define   POST                  1461726                 //POST
#define   PUT                   56374                   //PUT
#define   DELETE                840849585               //DELETE
#define   TRACE                 39872967                //TRACE
#define   CONNECT               198212747               //CONNECT

#define   CLOSE                 32008799                //close
#define   KEEP_ALIVE            1737230162              //keep-Alive

#define	  INVALID_VALUE		0

#define BASE   26
#define MOD    0xffffffff

#include <stdlib.h>
#include <ctype.h>

struct http_response
{
	int status;
	int status_string;
	int connection;
	int content_type;
	
	char *date;
	char *server;
	float version;
};

struct http_request
{
	int action;
	int connection;
	int accept_type;
	int content_type;
	int user_agent;
	int agent_os;
	int accept_lang;
	int accept_encoding;
	char *url;
	char *origin;
	char *host;
	char *extension;
	char *referrer;
	char *data;
	float version;
};

void pullback(char *from, int cnt)
{
	while(*from) {
		*(from-cnt) = *from;
		from++;
	}
	*(from-cnt) = '\0';
}

float get_http_version(char *str)
{
	float version;

	while(*str && *str != '/')
		str++;

	sscanf(++str, "%f", &version);

	return version;
}

#define CONV_HEX(n1) ((n1 >= '0' && n1 <= '9') ? n1 - '0' : toupper(n1) - 'A')
void decode_url(char *url, char **inlinedata)
{
	char n1, n2;

	while(*url) {

		if(*url == '?')
		{
			*url = '\0';
			*inlinedata = url + 1;
		}

		if(*url == '%') 
		{
			n1 = *(url+1);
			n2 = *(url+2);

			n1 = CONV_HEX(n1);
			n2 = CONV_HEX(n2);
			*url = ((n1 << 4) | n2);
			pullback(url+3, 2);
		}
		url++;
	}
}

void break_up(char *line, char **kvp)
{
	kvp[0] = line;
	kvp[1] = NULL;
	while(*line && *line != ':') line++;
	*line='\0'; line++;
	while(*line && (*line == ' ' || *line == '\t')) line++;
	kvp[1] = line;
}

char *gethttpline(char *stream, char **end)
{
	char *ret;
	
	while(*stream == '\r' || *stream == '\n') stream++;
	ret = stream;
	
	/* read RFC-2616 for line specs */
	do {
		while(*stream && *stream != '\r' && *stream != '\n')
			stream++;
		while(*stream && (*stream == '\n' || *stream == '\r'))
			stream++;
	} while(*stream == ' ' || *stream == '\t');
	*end = stream;
	stream--;
	while(*stream == '\n' || *stream == '\r')
		stream--;
	*(stream+1) = '\0';
	return ret[0] ? ret : NULL;
}

unsigned long hash(char *str)
{
	unsigned long hsh = 0;

	while(*str)
	{
		hsh = hsh * BASE + toupper(*str);
		str++;
	}
	return hsh % MOD;
}

char *gettoken(char *stream, char **pmarker, char *end)
{
	char *start = stream;
	while(*stream && !strchr(end, *stream)) stream++;
	*stream = 0;
	*pmarker = stream+1;
	return start;
}

void parse_httpheader(char *hstr, struct http_request *head)
{
	char *ptr, *end, *line, *temp;
	char *delims = "\r\n\t ";
	char *kvp[2];
	
	memset(head, 0, sizeof(struct http_request));	
	ptr = end = hstr;
	
	/* get the action */
	ptr = gettoken(end, &end, delims);
	
	switch((head->action = hash(ptr))) {
	   case GET: case POST: case PUT: case HEAD: break;
	   default: head->action = INVALID_VALUE;
	}
	
	/* get the URL */
	while(*end && (*end == '/' || *end == '.')) end++;
	head->url = gettoken(end, &end, delims);
	decode_url(head->url, &head->data);

	/* determine the extension */
	temp = strlen(head->url) + head->url - sizeof(char);
	while(*temp && *temp != '.') temp--;
	head->extension = temp + 1; 
	
	/* get the version */
	if(*end == 'H' && *(end+1) == 'T') {
		/*version is available fetch it.*/
		ptr = gettoken(end, &end, delims);
		head->version = get_http_version(ptr);
	}
	
	typedef unsigned long long int luint;
	
	/*check whether there is some data sent along with header */
	if(
		((luint)(ptr = strstr(end, "\r\n\r\n") + 4*sizeof(char)) != (luint)(sizeof(char)*4)) ||
		((luint)(ptr = strstr(end, "\n\n") + 2 * sizeof(char)) != (luint)(sizeof(char) * 2))
	) {
		if(!head->data) {
			head->data = ptr;
			*(ptr-1) = '\0';	/* detach data from the stream */
		}
	}
	
	/* Now there are multiple key value pairs next parse them*/
	while((line=gethttpline(end, &end))) {
		break_up(line, kvp);
		switch(hash(kvp[0])) {
		case CONNECTION:
			/* we support only this */
			head->connection = hash(kvp[1]);
			break;
		case DATE:
			break;
		case PRAGMA:
			break;
		case TRAILER:
			break;
		case TRANSFER_ENCODING:
			break;
		case UPGRADE:
			break;
		case VIA:
			break;
		case WARNING:
			break;
		default:
			break;
		}
	}
}
