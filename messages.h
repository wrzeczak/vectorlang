//------------------------------------------------------------------------------//
// MESSAGE - Macros for sending debug, error, warning, and todo messages simply //
// and with nice formatting. Written for and by wrzeczak 7/6/2025.              //
//------------------------------------------------------------------------------//

#pragma once

// TODO: try to see if i can make this error at compile-time rather than run-time
#define todo(l) printf("\nWRZ: TODO: function " l "() is not yet implemented!\n\n"); \
				exit(-2);
				
#ifdef DEBUG
#define dprint(s) printf("WRZ: DEBUG: " s "\n")
#define dprintf(s, ...) printf("WRZ: DEBUG: " s "\n", __VA_ARGS__)
#else
#define dprint(s)
#define dprintf(s, ...)
#endif

int messages_h_goto_exit = 0;

#ifndef MESSAGES_H_GOTONAME
	#ifndef MESSAGES_H_ERRORCODE
		#define eprint(c, s) do {	\
			printf("\nWRZ: ERROR: " s "\n\n");	\
			exit(c);	\
		} while(0)

		#define eprintf(c, s, ...) do { \
			printf("\nWRZ: ERROR: " s "\n\n", __VA_ARGS__); \
			exit(c);	\
		} while(0)
	#else
		#define eprint(s) do { \
			printf("\nWRZ: ERROR: " s "\n\n");	\
			exit(MESSAGES_H_ERRORCODE);		\
		} while(0)

		#define eprintf(s, ...) do { \
			printf("\nWRZ: ERROR: " s "\n\n", __VA_ARGS__); \
			exit(MESSAGES_H_ERRORCODE);	\
		} while(0)
	#endif
#else
	#ifndef MESSAGES_H_ERRORCODE
		#define eprint(c, s) do {	\
			printf("\nWRZ: ERROR: " s "\n\n");	\
			messages_h_goto_exit = 1;	\
			goto MESSAGES_H_GOTONAME;	\
		} while(0)

		#define eprintf(c, s, ...) do { \
			printf("\nWRZ: ERROR: " s "\n\n", __VA_ARGS__); \
			messages_h_goto_exit = 1;	\
			goto MESSAGES_H_GOTONAME;	\
		} while(0)
	#else
		#define eprint(s) do { \
			printf("\nWRZ: ERROR: " s "\n\n");	\
			messages_h_goto_exit = 1;	\
			goto MESSAGES_H_GOTONAME;		\
		} while(0)

		#define eprintf(s, ...) do { \
			printf("\nWRZ: ERROR: " s "\n\n", __VA_ARGS__); \
			messages_h_goto_exit = 1;	\
			goto MESSAGES_H_GOTONAME;	\
		} while(0)
	#endif
#endif

#define wprint(s) printf("WRZ: WARN: " s "\n")
#define wprintf(s, ...) printf("WRZ: WARN: " s "\n", __VA_ARGS__)