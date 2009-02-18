
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                              console.h
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#ifndef	_TINIX_CONSOLE_H_
#define	_TINIX_CONSOLE_H_

/* CONSOLE */
typedef struct s_console
{
	//struct s_tty*	p_tty;
	unsigned int	current_start_addr;	/* pos to disp */
	unsigned int	original_addr;	/* mem addr of current console */
	unsigned int	v_mem_limit;	/* mem size of current console */
	unsigned int	cursor;		/* cursor pos */
}CONSOLE;


#define SCROLL_SCREEN_UP	1	/* scroll forward */
#define SCROLL_SCREEN_DOWN	-1	/* scroll backward */

#define SCREEN_SIZE		(80 * 25)
#define SCREEN_WIDTH		80

#define DEFAULT_CHAR_COLOR	0x07	/* 0000 0111 fg: black, bg: white */


#endif /* _TINIX_CONSOLE_H_ */

