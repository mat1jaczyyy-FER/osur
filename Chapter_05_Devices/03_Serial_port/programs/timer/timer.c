/*! Timer api testing */

#include <stdio.h>
#include <time.h>
#include <api/prog_info.h>

static void print_time() {
	timespec_t t;
	clock_gettime(CLOCK_MONOTONIC, &t);
	printf("MT=%d:%d ", t.tv_sec, t.tv_nsec / 100000000);
	clock_gettime(CLOCK_REALTIME, &t);
	printf("RT=%d:%d\n", t.tv_sec, t.tv_nsec / 100000000);
}

static void alarm_nt(sigval_t param)
{
	printf("Alarm! ");
	print_time();
}

int timer()
{
	timespec_t t;
	itimerspec_t t1;
	timer_t timer1;
	sigevent_t evp;

	printf("Example program: [%s:%s]\n%s\n\n", __FILE__, __FUNCTION__,
		 timer_PROG_HELP);

	evp.sigev_notify = SIGEV_THREAD;
	evp.sigev_notify_function = alarm_nt;
	evp.sigev_notify_attributes = NULL;

	/* timer1 */
	t1.it_interval.tv_sec = 2;
	t1.it_interval.tv_nsec = 0;
	t1.it_value.tv_sec = 2;
	t1.it_value.tv_nsec = 0;
	evp.sigev_value.sival_int = t1.it_interval.tv_sec;
	timer_create(CLOCK_REALTIME, &evp, &timer1);
	timer_settime(&timer1, 0, &t1, NULL);

	while (1) {
		t.tv_sec = 5;
		t.tv_nsec = 0;

		while (TIME_IS_SET(&t))
			if (clock_nanosleep(CLOCK_MONOTONIC, 0, &t, &t))
				printf("Interrupted sleep?\n");

		printf("Rewind ");
		print_time();

		clock_gettime(CLOCK_REALTIME, &t);
		t.tv_sec -= 3;
		clock_settime(CLOCK_REALTIME, &t);

		printf("Now is ");
		print_time();
	}
	
	timer_delete(&timer1);

	return 0;
}
