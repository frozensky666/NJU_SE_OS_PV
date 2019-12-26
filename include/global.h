
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            global.h
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/* EXTERN is defined as extern except in global.c */
#ifdef GLOBAL_VARIABLES_HERE
#undef EXTERN
#define EXTERN
#endif

//My global vars --START
EXTERN int ticks;
EXTERN char currentProc;
EXTERN Semaphore rmutex; //读锁（用于写者优先）
EXTERN Semaphore x_lock; //rc锁
EXTERN Semaphore y_lock; //rw锁
EXTERN Semaphore wmutex; //写锁
EXTERN Semaphore s_lock; //用于公平算法
EXTERN Semaphore rmax;   //读者上限锁
EXTERN int Mode;         //当前模式（读者优先 0 /写者优先 1）
EXTERN int rc;           //readerCount
EXTERN int wc;           //writerCount
//My global vars --END

EXTERN int ticks;

EXTERN int disp_pos;
EXTERN u8 gdt_ptr[6]; // 0~15:Limit  16~47:Base
EXTERN DESCRIPTOR gdt[GDT_SIZE];
EXTERN u8 idt_ptr[6]; // 0~15:Limit  16~47:Base
EXTERN GATE idt[IDT_SIZE];

EXTERN u32 k_reenter;

EXTERN TSS tss;
EXTERN PROCESS *p_proc_ready;

extern PROCESS proc_table[];
extern char task_stack[];
extern TASK task_table[];
extern irq_handler irq_table[];
