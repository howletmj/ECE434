1.	Julia Cartwright works at National Instruments.
2.	PREEMPT_RT patch gives Linux real time capabilities by configuring the kernel and allowing for real time scheduling.
3.	Embedded systems have two types of tasks, tasks with latency requirements and tasks that are not as time dependent.
4.	Drivers can misbehave due to shared driver stacks
5.	Delta in figure 1 is the latency between an external event occurs and the time for an application starts.
6.	Cyclictest takes the difference in time stamps from a sleep function and determines the delta from figure 1.
7.	Figure 2 is a histogram of the latency samples.
8.	The dispatch latency is the time between the hardware firing and the relevant thread being woken up. Scheduling latency is the time it takes the scheduler to start the task.
9.	Mainline is the central version of linux that all versions are based off of.
10.	 A low priority interrupt prevents the external event from starting.
11.	Irq threads are used.  Minimal code is used in the interrupts in order to wake up the handler threads.
