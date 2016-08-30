#define IS_SIG_ON(p,signum)	(p->pending & (1<<signum))
#define TURN_ON(p,signum)	p->pending |= (1<<signum)
#define TURN_OFF(p,signum)	p->pending &= (~(1<<signum))

#define SIG_DEF	1

extern void defSigHandler(int);