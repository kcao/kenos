
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                tty.h
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#ifndef	_TINIX_TTY_H_
#define	_TINIX_TTY_H_


#define TTY_IN_BYTES	256	/* tty input queue size */

struct s_tty;
struct s_console;

/* TTY */
typedef struct s_tty
{
	t_32	in_buf[TTY_IN_BYTES];	/* TTY input buf*/
	t_32*	p_inbuf_head;	/* point to the next free pos in buf */
	t_32*	p_inbuf_tail;	/* point to the key val of keyboard task */
	int	inbuf_count;	/* count of used in buf */

	struct s_console *	p_console;
}TTY;


#endif /* _TINIX_TTY_H_ */

