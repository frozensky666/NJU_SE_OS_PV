
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                               proc.c
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

void next();
void wakeup(int proc);
void block();

//TODO
PUBLIC void schedule()
{ //时钟每中断一次，所有进程ticks减少，并且寻找下一个能运行的进程（轮转调度）
	for (PROCESS *p = proc_table; p < proc_table + NR_TASKS; p++)
		if (p->ticks > 0)
			p->ticks--;

	next();
}

PUBLIC int sys_get_ticks()
{
	return ticks;
}

PUBLIC void sys_disp_color_str(char *info, int color)
{
	disp_color_str(info, color);
	if (disp_pos >= 2 * 80 * 25)
	{
		// milli_delay(1000000000);
		clear();
	}
}

PUBLIC void sys_disp_str(char *info)
{
	disp_str(info);
	if (disp_pos >= 2 * 80 * 25)
	{

		// milli_delay(1000000000);
		clear();
	}
}

PUBLIC void sys_mydelay(int milli_sec)
{
	//增加 ticks（睡眠程序，且不占用时间片）
	p_proc_ready->ticks = milli_sec * HZ / 1000;
	//找到下一个能运行的程序
	next();
}

PUBLIC void sys_P(Semaphore *s, int proc)
{
	s->value--;
	if (s->value < 0)
	{
		s->Q[s->phead] = proc;
		block(); //阻塞当前程序并寻找下一能运行程序
		s->phead = (s->phead + 1) % Q_SIZE;
	}
}

PUBLIC void sys_V(Semaphore *s)
{
	s->value++;
	if (s->value <= 0)
	{
		int proc = s->Q[s->ptail];
		wakeup(proc); //唤醒队尾程序
		s->ptail = (s->ptail + 1) % Q_SIZE;
	}
}

//找到下一个ticks为0且没有被阻塞的程序
void next()
{
	while (1)
	{
		p_proc_ready = (p_proc_ready + 1 - proc_table) % NR_TASKS + proc_table;
		if (p_proc_ready->ticks <= 0 && p_proc_ready->blocked != 1)
			break;
	}
}
//唤醒程序，并把它置为当前运行程序
void wakeup(int proc)
{
	proc_table[proc].blocked = 0;
	p_proc_ready = proc_table + proc;
}
//将当前程序阻塞，并寻找下一个能运行的程序
void block()
{
	p_proc_ready->blocked = 1;
	next();
}
//清屏
PUBLIC void clear()
{
	int i = 0;
	disp_pos = 0;
	for (i = 0; i < 80 * 25; i++)
	{
		disp_str(" ");
	}
	disp_pos = 0;
}