#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdarg.h>

#include "em.h"
#include "68kint.h"
#include "cpudefs.h"

/*
Helpful links:
https://sourceware.org/gdb/current/onlinedocs/gdb/Packets.html
https://sourceware.org/gdb/current/onlinedocs/gdb/Overview.html
https://www.embecosm.com/appnotes/ean4/embecosm-howto-rsp-server-ean4-issue-2.html
*/

int listenPort = 15555;
int server=-1;
int conn=-1;
struct sockaddr_in clientAddress = {0};
int traceLink=1;
int noAck = 0;
static char in[16384], *inp=in;

void resetInput(void) {
	inp=in;
	noAck=0;
}

void sendGDB(char *fmt, ...) {
	if(conn<0) return;
	char m[2048];
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(m, sizeof(m), fmt, ap);
	va_end(ap);

	char buffer[2048], *p = buffer;
	char *s = m;
	*p++='$';
	int sum=0;
	int ch;
	while((ch = *s++)) {
//printf("%c %x+%x=%x\n", ch, sum, ch, sum+ch);
		if(ch=='}' || ch=='#' || ch=='$') {
			*p++ = '}';
			sum += '}';
			ch^=0x20;
		}
		*p++ = ch;
		sum+=ch;
	}
	sum&=0xff;
	p+=snprintf(p, 4, "#%02x", sum);
top:
	if(traceLink) printf("gdb send:%s\n", buffer);
	int len = p-buffer;
	int res=write(conn, buffer, len);
	if(res!=len) printf("WTF1!!!!!!!\n");
	char ans;
	if(!noAck) {
		res = read(conn, &ans, 1);res=res;
		if(traceLink>1) printf("RX %c\n", ans);
		if(ans=='+') return;
		goto top;
	}
}

int raiseTrap(int n) {
	if(n!=15) return 0;
	printf("raiseTrap %d\n", n);
	lastpc = -1;
	regs.pc -= 2;
	enterdebug();
	sendGDB("T02swbreak:;");
	return 1;
}


int match(char *got, char *want) {
	return strncmp(got, want, strlen(want))==0;
}
static void ok(void) {
	sendGDB("OK");
}

void processGDB(char *m) {
	int address=0, len=0, v=0, v2=0;
	char buf[4096] = {0};
	*buf = 0;
	char *p;
	int i;
	switch(*m) {
	case 'q':
		if(match(m, "qSupported")) {
			sendGDB("PacketSize=3fff;QStartNoAckMode+;swbreak+;hwbreak+");
		} else
			sendGDB("");
		break;
	case 'Q':
		if(match(m, "QStartNoAckMode")) {
			ok();
			noAck = 1;
		} else
			sendGDB("");
		break;
	case '?': // cause for haulting
		sendGDB("S05");
		break;
	case 'g': // registers
		sendGDB("%08X%08X%08X%08X%08X%08X%08X%08X%08X%08X%08X%08X%08X%08X%08X%08X%08X%08X",
		regs.d[0], regs.d[1], regs.d[2], regs.d[3], regs.d[4], regs.d[5], regs.d[6], regs.d[7],
		regs.a[0], regs.a[1], regs.a[2], regs.a[3], regs.a[4], regs.a[5], regs.a[6], regs.a[7],
		regs.sr, regs.pc);
		break;
	case 's': // single step
		stepone(1);
		sendGDB("S05");
		break;
	case 'm': // read from memory
		sscanf(m+1, "%x,%x", &address, &len);
		p=buf;
		for(i=0;i<len;++i) {
			p += snprintf(p, buf+sizeof(buf)-p, "%02x",
				cpu_readmem24(address+i));
		}
		sendGDB(buf);
		break;
	case 'X': // write to memory
		sscanf(m+1, "%x,%x:", &address, &len);
		char *p = index(m, ':');
		if(p) {
			++p;
			int i;
			for(i=0;i<len;++i) {
				cpu_writemem24(address+i, p[i]);
			}
		}
		ok();
		break;
	case 'p': // read register
		sscanf(m+1, "%d", &v);
		if(v>=0 && v<18) {
			if(v<8) v=regs.d[v];
			else if(v<16) v=regs.a[v-8];
			else if(v==16) v=regs.sr;
			else v=regs.pc;
			sprintf(buf, "%08x", v);
			sendGDB(buf);
		} else sendGDB("");
		break;
	case 'P': // write register
		sscanf(m+1, "%x=%x", &v, &v2);
		if(v>=0 && v<18) {
			if(v<8) regs.d[v]=v2;
			else if(v<16) regs.a[v-8]=v2;
			else if(v==16) regs.sr=v2;
			else regs.pc=v2;
			ok();
		} else sendGDB("");
		break;
	case 'c': // continue
		keymode=0;
		break;
	default: // unrecognized
		sendGDB("");
		break;
	}
}

void freeGDB(void) {
	if(server>=0) {
		printf("Closing gdb server port\n");
		close(server);
		server=-1;
	}
}
void initGDB(void) {
	struct sockaddr_in serverAddress = {0};
	server = socket(AF_INET, SOCK_STREAM, 0);
	int val = 1;
	setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = htonl(0x00000000);
	for(;;) {
		serverAddress.sin_port = htons(listenPort);
		int res = bind(server, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
		if(res>=0) break;
		++listenPort;
	}
	listen(server, 1);

	printf("gdb server listening on port %d\n", listenPort);
	atexit(freeGDB);
}
void pollGDB(void) {
	struct pollfd pfd = {server, POLLIN, 0};
	int res = poll(&pfd, 1, 0);
	if(res>0) {
		socklen_t len = sizeof(clientAddress);
		conn = accept(server, (struct sockaddr *)&clientAddress, &len);
printf("New gdb client connection, conn=%d\n", conn);
		resetInput();
		enterdebug();
	}
	if(conn<0) return;
	pfd = (struct pollfd) {conn, POLLIN|POLLERR, 0};
	res = poll(&pfd, 1, 0);
	if(res<=0) return;
	if(pfd.revents&POLLERR) {
		printf("gdb client %d closed\n", conn);
		close(conn);
		conn = -1;
		return;
	}

	int space = in+sizeof(in)-inp;
	int t = read(conn, inp, space);
	if(t>0) inp+=t;
	char *p = in;
	if(*p==3) {
		inp = in;
		enterdebug();
		sendGDB("S02");

		return;
	}
	while(p<inp && *p!='#') ++p;
	if(p==inp) return;
	if(p+3>inp) return; // waiting for #hh
	char msg[2048], *o=msg;
	int sum=0;
	char *s = in;
	if(*s=='+') {++s;if(trace) printf("Ignoring stray '+' on input\n");}
	if(*s=='$') ++s;
	else printf("WTF0!!!! %c\n", *s);
	while(s<p) {
		int ch = *s++;
		sum += ch;
		if(ch=='}' && s<p) {
			ch=*s++;
			sum += ch;
			ch ^= 0x20;
		}
		if(o<msg+sizeof(msg)-1) *o++ = ch;
	}
	*o = 0;
	sum&=0xff;
	int val=-1;
	sscanf(p+1, "%02x", &val);
	p+=3;
	int got = inp-p;
	if(sum!=val) {
		if(traceLink) {
			printf("Bad checksum on gdb message, expected %02x got %02x\n", val, sum);
			int res=write(1, in,inp-in);res=res;
			printf("\n");
		}
		char ans='-';
		res=write(conn, &ans, 1);
		if(res!=1) printf("WTF2!!!!!!!\n");
		if(traceLink>1) printf("gdb send:%c\n", ans);
// send back -
	} else {
		if(traceLink) printf("gdb recv:%s\n", msg);
		if(!noAck) {
			char ans='+';
			int res=write(conn, &ans, 1);
			if(res!=1) printf("WTF3!!!!!!!\n");
			if(traceLink>1) printf("gdb send:%c\n", ans);
		}
		processGDB(msg);
	}
	memmove(in, p, got);
	inp = in+got;
}
