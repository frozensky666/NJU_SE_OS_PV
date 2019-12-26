
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            main.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "type.h"
#include "const.h"
#include "protect.h"
#include "proto.h"
#include "string.h"
#include "proc.h"
#include "global.h"

//一些宏定义
#define MAX_READER 3
#define TIME 300
#define MODE 0
#define HUNGRY 1
//Declarations
PRIVATE void initSem(Semaphore *s, int value);
PRIVATE void printEndRead(char who);
PRIVATE void printEndWrite(char who);
PRIVATE void printStartRead(char who);
PRIVATE void printStartWrite(char who);
PRIVATE void printCurrent();
PRIVATE void read(char who);
/*======================================================================*
                            kern
							el_main
 *======================================================================*/
PUBLIC int kernel_main()
{
	disp_str("-----\"kernel_main\" begins-----\n");

	TASK *p_task = task_table;
	PROCESS *p_proc = proc_table;
	char *p_task_stack = task_stack + STACK_SIZE_TOTAL;
	u16 selector_ldt = SELECTOR_LDT_FIRST;
	int i;
	for (i = 0; i < NR_TASKS; i++)
	{
		strcpy(p_proc->p_name, p_task->name); // name of the process
		p_proc->pid = i;					  // pid

		p_proc->ldt_sel = selector_ldt;

		memcpy(&p_proc->ldts[0], &gdt[SELECTOR_KERNEL_CS >> 3],
			   sizeof(DESCRIPTOR));
		p_proc->ldts[0].attr1 = DA_C | PRIVILEGE_TASK << 5;
		memcpy(&p_proc->ldts[1], &gdt[SELECTOR_KERNEL_DS >> 3],
			   sizeof(DESCRIPTOR));
		p_proc->ldts[1].attr1 = DA_DRW | PRIVILEGE_TASK << 5;
		p_proc->regs.cs = ((8 * 0) & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
		p_proc->regs.ds = ((8 * 1) & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
		p_proc->regs.es = ((8 * 1) & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
		p_proc->regs.fs = ((8 * 1) & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
		p_proc->regs.ss = ((8 * 1) & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
		p_proc->regs.gs = (SELECTOR_KERNEL_GS & SA_RPL_MASK) | RPL_TASK;

		p_proc->regs.eip = (u32)p_task->initial_eip;
		p_proc->regs.esp = (u32)p_task_stack;
		p_proc->regs.eflags = 0x1202; /* IF=1, IOPL=1 */
		// p_proc->ticks = p_proc->blocked = 0;
		p_task_stack -= p_task->stacksize;
		p_proc++;
		p_task++;
		selector_ldt += 1 << 3;
	}

	// Init --Start
	proc_table[0].ticks = proc_table[0].blocked = 0;
	proc_table[1].ticks = proc_table[1].blocked = 0;
	proc_table[2].ticks = proc_table[2].blocked = 0;
	proc_table[3].ticks = proc_table[3].blocked = 0;
	proc_table[4].ticks = proc_table[4].blocked = 0;
	proc_table[5].ticks = proc_table[5].blocked = 0;

	initSem(&rmutex, 1);
	initSem(&wmutex, 1);
	initSem(&x_lock, 1);
	initSem(&y_lock, 1);
	initSem(&s_lock, 1);
	initSem(&rmax, MAX_READER);
	rc = wc = 0;
	Mode = MODE;
	clear();
	//Init --End

	k_reenter = 0;
	ticks = 0;
	p_proc_ready = proc_table;

	/* 初始化 8253 PIT */
	out_byte(TIMER_MODE, RATE_GENERATOR);
	out_byte(TIMER0, (u8)(TIMER_FREQ / HZ));
	out_byte(TIMER0, (u8)((TIMER_FREQ / HZ) >> 8));

	put_irq_handler(CLOCK_IRQ, clock_handler); /* 设定时钟中断处理程序 */
	enable_irq(CLOCK_IRQ);					   /* 让8259A可以接收时钟中断 */

	restart();

	while (1)
	{
	}
}

void ReaderA()
{
	int i = 0;
	while (1)
	{
		// printStartRead('A' + i);
		if (!HUNGRY)
			P(&s_lock, i);
		if (Mode == 1)
			P(&rmutex, i);
		P(&x_lock, i);
		if (rc == 0)
			P(&wmutex, i);
		rc++;
		V(&x_lock);
		if (Mode == 1)
			V(&rmutex);
		if (!HUNGRY)
			V(&s_lock);

		P(&rmax, i);
		printStartRead('A' + i);
		currentProc = 'A' + i;
		read('A' + i);
		milli_delay(2 * TIME);
		printEndRead('A' + i);
		V(&rmax);

		P(&x_lock, i);
		rc--;
		if (rc == 0)
		{
			V(&wmutex);
		}
		V(&x_lock);
	}
}

void ReaderB()
{
	int i = 1;
	while (1)
	{
		// printStartRead('A' + i);
		if (!HUNGRY)
			P(&s_lock, i);
		if (Mode == 1)
			P(&rmutex, i);
		P(&x_lock, i);
		if (rc == 0)
			P(&wmutex, i);
		rc++;
		V(&x_lock);
		if (Mode == 1)
			V(&rmutex);
		if (!HUNGRY)
			V(&s_lock);

		P(&rmax, i);
		printStartRead('A' + i);
		currentProc = 'A' + i;
		read('A' + i);
		milli_delay(3 * TIME);
		printEndRead('A' + i);
		V(&rmax);

		P(&x_lock, i);
		rc--;
		if (rc == 0)
		{
			V(&wmutex);
		}
		V(&x_lock);
	}
}

void ReaderC()
{
	int i = 2;
	while (1)
	{
		// printStartRead('A' + i);
		if (!HUNGRY)
			P(&s_lock, i);
		if (Mode == 1)
			P(&rmutex, i);
		P(&x_lock, i);
		if (rc == 0)
			P(&wmutex, i);
		rc++;
		V(&x_lock);
		if (Mode == 1)
			V(&rmutex);
		if (!HUNGRY)
			V(&s_lock);

		P(&rmax, i);
		printStartRead('A' + i);
		currentProc = 'A' + i;
		read('A' + i);
		milli_delay(3 * TIME);
		printEndRead('A' + i);
		V(&rmax);

		P(&x_lock, i);
		rc--;
		if (rc == 0)
		{
			V(&wmutex);
		}
		V(&x_lock);
	}
}

void WriterD()
{
	int i = 3;
	while (1)
	{
		// printStartWrite('A' + i);
		if (!HUNGRY)
			P(&s_lock, i);
		if (Mode == 1)
		{
			P(&y_lock, i);
			wc++;
			if (wc == 1)
				P(&rmutex, i);
			V(&y_lock);
		}

		P(&wmutex, i);
		printStartWrite('A' + i);
		currentProc = 'A' + i;
		read('A' + i);
		milli_delay(3 * TIME);
		printEndWrite('A' + i);
		V(&wmutex);

		if (Mode == 1)
		{
			P(&y_lock, i);
			wc--;
			if (wc == 0)
				V(&rmutex);
			V(&y_lock);
		}
		if (!HUNGRY)
			V(&s_lock);
	}
}

void WriterE()
{
	int i = 4;
	while (1)
	{
		// printStartWrite('A' + i);
		if (!HUNGRY)
			P(&s_lock, i);
		if (Mode == 1)
		{
			P(&y_lock, i);
			wc++;
			if (wc == 1)
				P(&rmutex, i);
			V(&y_lock);
		}

		P(&wmutex, i);
		printStartWrite('A' + i);
		currentProc = 'A' + i;
		read('A' + i);
		milli_delay(4 * TIME);
		printEndWrite('A' + i);
		V(&wmutex);

		if (Mode == 1)
		{
			P(&y_lock, i);
			wc--;
			if (wc == 0)
				V(&rmutex);
			V(&y_lock);
		}
		if (!HUNGRY)
			V(&s_lock);
	}
}

void F()
{
	while (1)
	{
		mydelay(1 * TIME);
		printCurrent();
	}
}

//其他工具函数
PRIVATE void initSem(Semaphore *s, int value)
{
	s->phead = s->ptail = 0;
	s->value = value;
}

PRIVATE void read(char who)
{
	char out[20] = "  is Reading!\n\0";
	char p = who;
	out[0] = p;
	color_print(out, WHITE);
}

PRIVATE void printEndRead(char who)
{
	char out[20] = "Reader   End!\n\0";
	char p = who;
	out[7] = p;
	color_print(out, RED);
}

PRIVATE void printEndWrite(char who)
{
	char out[20] = "Writer   End!\n\0";
	char p = who;
	out[7] = p;
	color_print(out, GREEN);
}

PRIVATE void printStartRead(char who)
{
	char out[20] = "Reader   Start!\n\0";
	char p = who;
	out[7] = p;
	color_print(out, RED);
}

PRIVATE void printStartWrite(char who)
{
	char out[20] = "Writer   Start!\n\0";
	char p = who;
	out[7] = p;
	color_print(out, GREEN);
}

PRIVATE void printCurrent()
{
	if (currentProc == 'A' || currentProc == 'B' || currentProc == 'C')
	{
		char out[20] = "Now Reading:  \n\0";
		out[13] = '0' + rc; //TODO
		// out[13] = currentProc;
		print(out);
	}
	else if (currentProc == 'D' || currentProc == 'E')
	{
		char out[20] = "Now Writing:  \n\0";
		out[13] = currentProc;
		print(out);
	}
}