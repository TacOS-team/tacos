#include <signal.h>
#include <syscall.h>

int kill(unsigned int pid, int sig)
{
	int ret;
	
	syscall(SYS_KILL, (uint32_t) pid, (uint32_t) sig, (uint32_t) &ret);
	
	return ret;
}


sighandler_t signal(int sig, sighandler_t func)
{
	sighandler_t ret;
	
	syscall(SYS_SIGNAL, (uint32_t)sig, (uint32_t)func, (uint32_t)&ret);
	
	return ret;
}

int sigprocmask(int how, const sigset_t *set, sigset_t *oldset)
{
	int ret = -1;
	if(set != NULL)
	{
		ret = 0;
		syscall(SYS_SIGPROCMASK, (uint32_t)how, (uint32_t)set, (uint32_t)oldset);
	}
	return ret;
}


int sigemptyset(sigset_t *set)
{
	int ret = -1;
	if(set != NULL)
	{
		ret = 0;
		*set = 0;
	}
	return ret;
}

int sigfillset(sigset_t *set)
{
	int ret = -1;
	if(set != NULL)
	{
		ret = 0;
		*set = 0xFFFFFFFF;
	}
	return ret;
}

int sigaddset(sigset_t *set, int signum)
{
	int ret = -1;
	if(set != NULL)
	{
		ret = 0;
		*set |= (1<<signum);
	}
	return ret;
}

int sigdelset(sigset_t *set, int signum)
{
	int ret = -1;
	if(set != NULL)
	{
		ret = 0;
		*set &= ~(1<<signum);
	}
	return ret;
}

int sigismember(const sigset_t *set, int signum)
{
	int ret = -1;
	if(set != NULL)
		ret = 0x1 & (*set>>signum);
	
	return ret;
}
	

