/*! Simple program prepared for little debugging */

#include <stdio.h>
#include <api/prog_info.h>
#include <api/errno.h>
#include <kernel/errno.h>

static int inc(int n)
{
	n++;

	return n;
}
int x = 5;
int y[100];
const int d = 123;

int debug()
{
	int a, b, c;
	a = 1;

	b = a + 1;

	c = inc(a) + inc(b);

	a += b + c;
	b += a + c;
	c += a + b;

#if 1	/* compile with 'debug=yes' and without */
	void k_startup();
	extern char system_info[];
	extern void premjesti();

	LOG ( INFO, "Address of premjesti (loader) is     %x", premjesti );
	LOG ( INFO, "Address of k_startup (krn code) is   %x", k_startup );
	LOG ( INFO, "Address of system_info (krn data) is %x", system_info );
	LOG ( INFO, "Address of 'debug' (prog code) is    %x", debug );

	LOG ( INFO, "Address of 'a' (stack variable) is   %x", &a );
	LOG ( INFO, "Address of 'd' (read-only const) is  %x", &d );
	LOG ( INFO, "Address of 'x' (global variable) is  %x", &x );
	//LOG ( INFO, "Address of 'y' is %x", y );
	
	extern int xxxx;
	LOG ( INFO, "  Value of 'xxxx' is %x", xxxx );
	LOG ( INFO, "Address of 'xxxx' is %x", &xxxx );

#endif
	return 0;
}
