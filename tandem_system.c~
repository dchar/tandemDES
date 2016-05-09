/* External definitions for single-server queueing system. */

#include <stdio.h>  
#include <stdlib.h>
#include <math.h>
#include "lcgrand.h"  /* Header file for random-number generator. */

#define Q_LIMIT 100  /* Limit on queue length. */
#define BUSY      1  /* Mnemonics for server's being busy */
#define IDLE      0  /* and idle. */

int   next_event_type, num_custs_delayed, num_events, num_in_q[2], server_status[2];
float area_num_in_q[2], area_server_status[2], mean_interarrival, mean_service[2],
      sim_time, time_end, time_arrival[Q_LIMIT + 1], second_time_arrival[Q_LIMIT + 1], 
	  time_last_event, time_next_event[6], total_of_delays;
FILE  *infile, *outfile, *debugfile;

void  initialize(void);
void  timing(void);
void  system_arrival(void);
void  queue1_departure(void);
void  queue2_arrival(void);
void  system_departure(void);
void  report(void);
void  update_time_avg_stats(void);
float expon(float mean);


main()  /* Main function. */
{
    /* Open input and output files. */

    infile  = fopen("tandem.in",  "r");
    outfile = fopen("tandem.out", "w");
	debugfile = fopen("debug.out", "w");

    /* Specify the number of events for the timing function. */

    num_events = 5;

    /* Read input parameters. */

    fscanf(infile, "%f %f %f %f", &mean_interarrival, &mean_service[0],
           &mean_service[1], &time_end);

    /* Write report heading and input parameters. */

    fprintf(outfile, "Tandem-server queueing system\n\n");
    fprintf(outfile, "Mean interarrival time%11.3f minutes\n\n",
            mean_interarrival);
    fprintf(outfile, "SRVR1 mean service time%16.3f minutes\n\n", mean_service[0]);
	fprintf(outfile, "SRVR2 mean service time%16.3f minutes\n\n", mean_service[1]);
    fprintf(outfile, "Length of the simulation%16.3f minutes\n\n", time_end);

	int i;

	/* Run simulation ten times total */
	for (i = 0; i < 10; i++) {
      /* Initialize the simulation. */
	
      initialize();

      /* Run the simulation until the end time is reached */
      do {
        /* Determine the next event. */
        timing();

        /* Update time-average statistical accumulators. */
        update_time_avg_stats();

		/* FIXME log loop information to debug file */
		fprintf(debugfile, "\nCALL:%d    TIME:%f\n", next_event_type, sim_time);
    	fprintf(debugfile, "#Q1 :%d    #Q2 :%d\n",
            num_in_q[0], num_in_q[1]);
    	fprintf(debugfile, "SRV1:%d    SRV2:%d\n",
            server_status[0], server_status[1]);

        /* Invoke the appropriate event function. */
        switch (next_event_type) 
        {
            case 1:
                system_arrival();
                break;
            case 2:
                system_departure();
                break;
			case 3:
				queue1_departure();
				break;
			case 4:
				queue2_arrival();
				break;
			case 5:
				report();
				break;
        }
	  /* If the event just executed was not the end-simulation event, then continue */
	  } while (next_event_type != 5);
	}

    fclose(infile);
    fclose(outfile);

    return 0;
}


void initialize(void)  /* Initialization function. */
{
	int i;

    /* Initialize the simulation clock. */
    sim_time = 0.0;

    /* Initialize the state variables and statistical counters. */
	for (i = 0; i < 2; i++) {
		server_status[i]      = IDLE;		
		num_in_q[i]           = 0;
    	area_num_in_q[i]      = 0.0;
    	area_server_status[i] = 0.0;
	}

    num_custs_delayed  = 0;
    total_of_delays    = 0.0;
    time_last_event    = 0.0;

    /* Initialize event list.  Since no customers are present, the departure
       (service completion) and tandem switch events are eliminated from consideration. */
    time_next_event[1] = sim_time + expon(mean_interarrival);
    time_next_event[2] = 1.0e+30;
	time_next_event[3] = 1.0e+30;
	time_next_event[4] = 1.0e+30;
	time_next_event[5] = time_end;
}


void timing(void)  /* Timing function. */
{
    int   i;
    float min_time_next_event = 1.0e+29;

    next_event_type = 0;

    /* Determine the event type of the next event to occur. */
    for (i = 1; i <= num_events; ++i)
        if (time_next_event[i] < min_time_next_event)
        {   
            min_time_next_event = time_next_event[i];
            next_event_type     = i;
        }

    /* Check to see whether the event list is empty. */
    if (next_event_type == 0)
    {
        /* The event list is empty, so stop the simulation. */
        fprintf(outfile, "\nEvent list empty at time %f", sim_time);
        exit(1);
    }

    /* The event list is not empty, so advance the simulation clock. */
    sim_time = min_time_next_event;
}


void system_arrival(void)  /* Arrive in the system (queue one) */
{
    float delay;

    /* Schedule next arrival. */
    time_next_event[1] = sim_time + expon(mean_interarrival);

    /* Check to see whether server is busy. */
    if (server_status[0] == BUSY) {
        /* Server is busy, so increment number of customers in the first queue. */
        ++num_in_q[0];

        /* Check to see whether an overflow condition exists. */
        if (num_in_q[0] > Q_LIMIT) {
            /* The queue has overflowed, so stop the simulation. */
            fprintf(outfile, "\nOverflow of the first array time_arrival at");
            fprintf(outfile, " time %f \n\n", sim_time);
            exit(2);
        }

        /* There is still room in the queue, so store the time of arrival of the
           arriving customer at the (new) end of time_arrival. */
        time_arrival[num_in_q[0]] = sim_time;
    }

    else {
        /* Server is idle, so arriving customer has a delay of zero.*/
        delay            = 0.0;
        total_of_delays += delay;

        /* Increment the number of customers delayed, and make server busy. */
        ++num_custs_delayed;
        server_status[0] = BUSY;

        /* Schedule arrival in second queue */
        time_next_event[3] = sim_time + expon(mean_service[0]);
    }
}


/* Tandem-queue switching function. Removes the customer from the first service
   queue, then adds the customer to the second service queue and schedules departure */

void queue1_departure(void) 
{
	int i;
	float delay;

	/* Check to see whether the first queue is empty */
	if (num_in_q[0] == 0) {
		/* The first queue is empty so make the server idle */
		server_status[0]   = IDLE;
		time_next_event[3] = 1.0e+30;
	}
	
	/* Decrement the number of customers in the first queue. */
	else {
		--num_in_q[0];

        /* Compute the delay of the customer who is beginning service and update
           the total delay accumulator. */
        delay            = sim_time - time_arrival[1];
        total_of_delays += delay;

		/* Increment number of customers delayed and schedule queue 2 arrival */
		++num_custs_delayed;
		server_status[0] = BUSY;
		time_next_event[4] = sim_time + expon(mean_service[0]);

        /* Move each customer in queue (if any) up one place. */
        for (i = 1; i <= num_in_q[0]; ++i)
            time_arrival[i] = time_arrival[i + 1];

		/* FIXME logging to debug file */		
		fprintf(debugfile, "SCHEDULING 4 | time:%f\n", time_next_event[4]);		
	}

}

void queue2_arrival(void) /* Arrive at the second queue */
{
	float delay;

	/* Wait for the next arrival afterward*/
	time_next_event[4] = 1.0e+30;

	/* Check to see whether the second server is busy. */
	if (server_status[1] == BUSY) {
		/* Second server is busy, so increment the number of customers in
		 * the second queue. */
		++num_in_q[1];

		if (num_in_q[1] > Q_LIMIT) {
			/* The second queue has overflowed; stop the simulation. */	
            fprintf(outfile, "\nOverflow of the second array time_arrival at");
            fprintf(outfile, " time %f \n\n", sim_time);
            exit(2);
		}

		/* There is still room in the queue, so so store the time of arrival of the
		 * switching customer at the end of the second_time_arrival array.  */
		second_time_arrival[num_in_q[1]] = sim_time;
	}

	else {
		/* Second server is idle; current customer has delay of 0 */
		delay			 = 0.0;
		total_of_delays += delay;

		/* Make second server busy, but do not increment number of customers delayed */
		server_status[1] = BUSY;

		/* Schedule system departure for the current customer*/
		time_next_event[2] = sim_time + expon(mean_service[1]);

		/* FIXME logging to debug file */
		fprintf(debugfile, "SCHEDULING 2 | time:%f\n", time_next_event[2]);
	}
}

void system_departure(void)  /* Departure event function. */
{
    int   i;
    float delay;

    /* Check to see whether the queue is empty. */
    if (num_in_q[1] == 0) {
        /* The queue is empty so make the server idle and eliminate the
           departure (service completion) event from consideration. */
        server_status[1]      = IDLE;
        time_next_event[2]    = 1.0e+30;
    }

    else {
        /* The queue is nonempty, so decrement the number of customers in
           queue. */
        --num_in_q[1];

        /* Compute the delay of the customer who is beginning service and update
           the total delay accumulator. */
        delay            = sim_time - second_time_arrival[1];
        total_of_delays += delay;

        /* Increment the number of customers delayed, and schedule departure. */
        ++num_custs_delayed;
		server_status[1] = BUSY;
        time_next_event[2] = sim_time + expon(mean_service[1]);

        /* Move each customer in queue (if any) up one place. */
        for (i = 1; i <= num_in_q[1]; ++i)
            second_time_arrival[i] = second_time_arrival[i + 1];
    }
}


void report(void)  /* Report generator function. */
{
    /* Compute and write estimates of desired measures of performance. */

    fprintf(outfile, "\n\nAverage delay in queue%11.3f minutes\n\n",
            total_of_delays / num_custs_delayed);
    fprintf(outfile, "Average number in queue 1:%10.3f\n\n",
            area_num_in_q[0] / sim_time);
    fprintf(outfile, "Average number in queue 2:%10.3f\n\n",
            area_num_in_q[1] / sim_time);
    fprintf(outfile, "SRVR1 utilization%15.3f\n\n",
            area_server_status[0] / sim_time);
    fprintf(outfile, "SRVR2 utilization%15.3f\n\n",
            area_server_status[1] / sim_time);
    fprintf(outfile, "Time simulation ended%12.3f minutes\n\n", sim_time);
}


void update_time_avg_stats(void)  /* Update area accumulators for time-average
                                     statistics. */
{
	int   i;
    float time_since_last_event;

    /* Compute time since last event, and update last-event-time marker. */
    time_since_last_event = sim_time - time_last_event;
    time_last_event       = sim_time;

    /* Update area under number-in-queue and server-busy indicator function. */
	for(i = 0; i < 2; i++) {
    	area_num_in_q[i]      += num_in_q[i] * time_since_last_event;
    	area_server_status[i] += server_status[i] * time_since_last_event;
	}
}


float expon(float mean)  /* Exponential variate generation function. */
{
    /* Return an exponential random variate with mean "mean". */

    return -mean * log(lcgrand(1));
}

