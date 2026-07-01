
#ifndef _DNS_H_
#define _DNS_H_

#include <stdio.h>

#include <typen.h>
#include <debug.h>


#define MAXNAMELEN 255


/* rfc1035 - page 26,27 */
typedef struct {
        DNS_USI   id;        /* uniq id */
        DNS_UI    qr:1;      /* query = 0  -  answer = 1 */
        DNS_UI    opcode:4;  /* kind of query (0=query; 1=inverse query; 2=server status query)*/
        DNS_UI    aa:1;      /* authoritative answer */
        DNS_UI    tc:1;      /* message truncation */
        DNS_UI    rd:1;      /* recursive query */
        DNS_UI    ra:1;      /* recursion available */
        DNS_UI    z:3;       /* reserved */
        DNS_UI    rcode:4;   /* response code */
        DNS_USI   qdcount;   /* number of entries in the question section */
        DNS_USI   ancount;   /* number of entries in the answer section */
        DNS_USI   nscount;   /* number of entries in the authority section */
        DNS_USI   arcount;   /* number of entries in the additional records section */
}DNSHeader;
#define DNSHEADERLEN (12)                 /* std. header size in msg. */
#define DNSHEADERSIZE (sizeof(DNSHeader)) /* struct size */


/* rfc1035 - page 28 */
typedef struct {
        char     qname[256];
        DNS_USI  qtype;
        DNS_USI  qclass;
}DNSQuestion;
#define DNSQUESTIONSIZE (sizeof(DNSQuestion))


/* rfc1035 - page 29 (auﬂer "rrlen") */
typedef struct {
        char       name[256];
        DNS_USI    type;
        DNS_USI    class;
        int        ttl;
        DNS_USI    rdlength;
        char     * rdata;
        DNS_USI    rrlen;
}DNSrr;
#define DNSRRSIZE (sizeof(DNSrr))

/* rfc1035 - page 12,13 */
enum rrtype {
                      RRT_A = 1, /* host address */
                      RRT_NS,    /* an authoritative name server */
                      RRT_MD,    /* a mail destination (OBSOLETE) use MX */
                      RRT_MF,    /* a mail forwarder (OBSOLETE) use MX */
                      RRT_CNAME, /* the canonical name for an alias */
                      RRT_SOA,   /* marks the state of authority */
                      RRT_MB,    /* a mailbox domain name (EXPERIMENTAL) */
                      RRT_MG,    /* a mail group member (EXPERIMENTAL) */
                      RRT_MR,    /* a mail rename domain name (EXPERIMENTAL) */
                      RRT_NULL,  /* a null RR (EXPERIMENTAL)*/
                      RRT_WKS,   /* a well known service description */
                      RRT_PTR,   /* a domain name pointer */
                      RRT_HINFO, /* host information */
                      RRT_MINFO, /* mailbox or maillist information */
                      RRT_MX,    /* mail exchange */
                      RRT_TXT,   /* text strings */
                      
                      /* qtype */
                      RRT_AXFR  = 252, /* a request for a transfer of an entire zone */
                      RRT_MAILB = 253, /* a request for mailbox-related records (MB, MG or MR)*/
                      RRT_MAILA = 254, /* a request for mail agent RRs (OBSOLETE - see MX) */
                      RRT_ALL   = 255  /* a request for all records */
            };
/* rfc1035 - page 13 */
enum rrclass {
                      RRC_IN = 1,   /* the internet */
                      RRC_CS,       /* the CSNET class (OBSOLETE - used only for examples in some obsolete RFCs) */
                      RRC_CH,       /* the CHAOS class */
                      RRC_HS,       /* Hesiod (Dyer 87) */
                      
                      /* qclass */ 
                      RRC_ALL = 255 /* any class */
             };

/* rfc1035 - page 26 */
enum opcode {
                      OPCODE_QUERY = 0, /* Normale Query */
                      OPCODE_IQUERY,    /* Inverse Query */
                      OPCODE_STATUS     /* Status Anfrage */
            };

/* rfc1035 - page 27 */
enum rcode {
                      RCODE_NO_ERROR = 0,    /* Kein Fehler */
                      RCODE_FORMAT_ERROR,    /* Frage nicht auswertbar */
                      RCODE_SERVER_FAILURE,  /* Server Fehler */
                      RCODE_NAME_ERROR,      /* Name existiert nicht */
                      RCODE_NOT_IMPLEMENTET, /* Nicht implementierte Anfrage */
                      RCODE_REFUSED          /* Nicht erlaubt */
           };

#include <debug.h>
int iniDNSlib( char * host, char * user, char * pass, char * db, DNS_USI port, 
               int eDebugLevel, LogFunktion logCallback, char * fwd );
void deIniDNSlib(void);

int processDNSquery( char ** msg, size_t * msgLen );
int processDNSqueryEntry( char ** msg, size_t * msgLen, DNSQuestion * q, DNSHeader * h );

int extractHeader( char * msg, int msglen, DNSHeader * header );
int extractQuestion( char * msg, int msglen, DNSQuestion * question );

int codeHeader( char * msg, DNSHeader * header );

int convertDomainName( char * dnIn, char * dnOut, int direction );
void strConvert( char * in, char * out, char replace, char ers );


int appendRR( DNSrr * rr, char * msg, int pos );

void printHeader( DNSHeader * header, FILE * stream );
void printQuestion( DNSQuestion * question, FILE * stream );
void printRR( DNSrr * rr, FILE * stream );

#endif

/*EOF*/
