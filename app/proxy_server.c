#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <cnaiapi.h>
#include <arpa/inet.h>

#if defined(LINUX) || defined(SOLARIS)
	#include <sys/time.h>
#endif

#define BUFFSIZE    8192
#define PORT 20000
#define DEF_PORT 80

typedef void sigfunc(int);

int	recvln(connection, char *, int);
void send_head(connection, int, int);
void int_to_ip(int remote_comp, unsigned char *bytes);
//sigfunc *signal(int, sigfunc*);

int main(int argc, char *argv[])
{
	FILE *fp;
    connection conn, tcp_conn;
	int n;
	char buff[BUFFSIZE], cmd[16], path[64], vers[16];
	char *timestr;
    computer remote_comp;
	unsigned char bytes[4] = {0};
	//struct sigaction sa;

    #if defined(LINUX) || defined(SOLARIS)
	    struct timeval tv;
    #elif defined(WIN32)
	    time_t tv;
    #endif

    while(1){

        conn = await_contact((appnum) PORT);
        if (conn < 0)
		    exit(1);
        
		memset(buff, 0, BUFFSIZE);
        n = recvln(conn, buff, BUFFSIZE);
		
		sscanf(buff, "%s %s %s", cmd, path, vers);
		

		char *request;

		if(strstr(path, "http://"))
			request = &(path[7]);
		else if(strstr(path, "https://"))
			request = &(path[8]);
		
		fprintf(stdout, "%s\n", request);
		fflush(stdout);

		char *addr = malloc(strlen(request)+1);
		char *temp_host = malloc(strlen(request)+1);
		strcpy(temp_host, request);

		char *temp = index(request,':');
		if (temp == NULL)
			request = (char *) strtok(request, "/");
		else
			request = (char *) strtok(request, ":");

		char *dummy = NULL;
    	dummy = (char *) strtok(temp_host, "/");
		dummy = (char *) strstr(temp_host, ":");

		
		#if defined(LINUX) || defined(SOLARIS)
			gettimeofday(&tv, NULL);
			timestr = ctime(&tv.tv_sec);
        #elif defined(WIN32)
			time(&tv);
			timestr = ctime(&tv);
        #endif

		int port;
        memset(buff, 0, BUFFSIZE);
		
		if (dummy == NULL){
			sprintf(buff,"%s %s %d", timestr, request, DEF_PORT);
			port = DEF_PORT;
		}
		else{
			sprintf(buff,"%s %s %s", timestr, request, dummy);
			++dummy;
			port = atoi(dummy);
		}

		remote_comp = cname_to_comp(request);
		if(remote_comp == -1){
			continue;
		}
			
		int_to_ip(remote_comp, bytes);
		
		tcp_conn = make_contact(remote_comp, port);
		if(tcp_conn < 0)
			exit(1);

		memset(buff, 0, BUFFSIZE);
		snprintf(buff, sizeof(buff),"GET / HTTP/1.1\r\nHost: %s\r\nUser-Agent: PROXY_V4570 0.1\r\n\r\n", request);
		fprintf(stdout,buff);
		fflush(stdout);
		
		send(tcp_conn, buff, strlen(buff), 0);
		send_eof(tcp_conn);
		memset(buff, 0, BUFFSIZE);
		
		long response_len = 0;
		memset(buff, 0, BUFFSIZE);

		while(n=recvln(tcp_conn, buff, BUFFSIZE-1) > 0){
			response_len += n;
			if(buff == NULL)
				break;
			buff[BUFFSIZE-1] = '\0';
			send(conn, buff,strlen(buff),0);
			memset(buff, 0, BUFFSIZE);
		}
		send_eof(conn);
        
		fp = fopen("log.txt", "a");
		fprintf(fp, "%s %s", timestr, request);
		fflush(fp);
		fclose(fp);
    }
}

// Converts a int hostname to a dotted ip
void int_to_ip(int remote_comp, unsigned char *bytes){
	
    bytes[0] = (remote_comp >> 0) & 0xFF;
    bytes[1] = (remote_comp >> 8) & 0xFF;
    bytes[2] = (remote_comp >> 16) & 0xFF;
    bytes[3] = (remote_comp >> 24) & 0xFF;   
}