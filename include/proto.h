
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            proto.h
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/* klib.S */
PUBLIC void out_byte(t_port port, t_8 value);
PUBLIC t_8  in_byte(t_port port);
PUBLIC void disp_str(char * info);
PUBLIC void disp_color_str(char * info, int color);
PUBLIC void disable_int();
PUBLIC void enable_int();

/* protect.c */
PUBLIC void init_prot();
PUBLIC t_32 seg2phys(t_16 seg);
PUBLIC void disable_irq(int irq);
PUBLIC void enable_irq(int irq);

/* klib.c */
PUBLIC void delay(int time);
PUBLIC void disp_int(int input);
PUBLIC t_bool is_alphanumeric(char ch);

/* kernel.S */
void restart();

/* main.c */
void TestA();
void TestB();
void TestC();

/* i8259.c */
PUBLIC void put_irq_handler(int iIRQ, t_pf_irq_handler handler);
PUBLIC void spurious_irq(int irq);

/* clock.c */
PUBLIC void clock_handler(int irq);
PUBLIC void milli_delay(int milli_sec);

/* proc.c */
PUBLIC void sched0();

/* manmod.c */
PUBLIC	int manmod(); 

/* sfile.S */
PUBLIC void smodf();

/* keyboard.c */
PUBLIC void keyboard_handler(int irq);
PUBLIC void keyboard_read(TTY* p_tty);

/* tty.c */
PUBLIC void task_tty();
PUBLIC void in_process(TTY* p_tty, t_32 key);
PUBLIC void tty_write(TTY* p_tty, char* buf, int len);

/* console.c */
PUBLIC void	init_screen(TTY* p_tty);
PUBLIC void	out_char(CONSOLE* p_con, char ch);
PUBLIC void	scroll_screen(CONSOLE* p_con, int direction);
PUBLIC t_bool	is_current_console(CONSOLE* p_con);

/* printf.c */
PUBLIC	int	printf(const char *fmt, ...);

/* vsprintf.c */
PUBLIC	int	vsprintf(char *buf, const char *fmt, va_list args);


/* protoytpes related to system calls */
/*------------*/
/*   system   */
/*------------*/

/* proc.c */
PUBLIC	int	sys_get_ticks	();
PUBLIC	int	sys_write	(char* buf, int len, PROCESS* p_proc);
/* add more sys_call here */

/* xsched.c */
PUBLIC	int	sys_sw_sched	(); 

/* syscall.S */
PUBLIC	void	sys_call();	/* t_pf_int_handler */


/*------------*/
/*    user    */
/*------------*/

/* syscall.S */
PUBLIC	int	get_ticks();
PUBLIC	void	write(char* buf, int len);
PUBLIC	int	sw_sched();
/* add more sys_call here */



