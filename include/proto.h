
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            proto.h
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/* klib.asm */
PUBLIC void out_byte(u16 port, u8 value);
PUBLIC u8 in_byte(u16 port);
PUBLIC void disp_str(char *info);
PUBLIC void disp_color_str(char *info, int color);

/* protect.c */
PUBLIC void init_prot();
PUBLIC u32 seg2phys(u16 seg);

/* klib.c */
PUBLIC void delay(int time);

/* kernel.asm */
void restart();

/* main.c */
void ReaderA();
void ReaderB();
void ReaderC();
void WriterD();
void WriterE();
void F();

/* i8259.c */
PUBLIC void put_irq_handler(int irq, irq_handler handler);
PUBLIC void spurious_irq(int irq);

/* clock.c */
PUBLIC void clock_handler(int irq);
PUBLIC void milli_delay(int milli_sec);

/* Semaphore */
#define Q_SIZE 6
typedef struct Semaphore
{
    int value;
    int Q[Q_SIZE];
    int phead;
    int ptail;
} Semaphore;

/* 以下是系统调用相关 */

/* proc.c */
PUBLIC void clear();

PUBLIC int sys_get_ticks();
PUBLIC void sys_disp_str(char *info);
PUBLIC void sys_disp_color_str(char *info, int color);
PUBLIC void sys_P(Semaphore *s, int proc);
PUBLIC void sys_V(Semaphore *s);
PUBLIC void sys_mydelay(int milli_sec);

/* syscall.asm */
PUBLIC void sys_call(); /* int_handler */
PUBLIC int get_ticks();
PUBLIC void mydelay(int milli_sec);
PUBLIC void print(char *info);
PUBLIC void color_print(char *info, int color);
PUBLIC void P(Semaphore *s, int proc);
PUBLIC void V(Semaphore *s);