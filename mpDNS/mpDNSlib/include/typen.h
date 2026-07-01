
#ifndef _TYPEN_H_
#define _TYPEN_H_

#if (defined(_WIN32) || defined(__CYGWIN__))

#define DNS_I   int                 /* 32bit signed integer */
#define DNS_UI  unsigned int        /* 32bit unsigned signed integer */
#define DNS_USI unsigned short int  /* 16bit unsigned integer */
#define DNS_UCH unsigned char       /* 8bit unsigned char */

#else

#include <stdint.h>
#define DNS_I   int32_t   /* 32bit signed integer */
#define DNS_UI  uint32_t  /* 32bit unsigned signed integer */
#define DNS_USI uint16_t  /* 16bit unsigned integer */
#define DNS_UCH uint8_t   /* 8bit unsigned char */

#endif

#endif

/*EOF*/
