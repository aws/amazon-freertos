Assuming the xQueue is allocated to a valid memory block and abstracting
away concurrency and task pool related functions, this harness proves the memory
safety of QueueGiveFromISR.
