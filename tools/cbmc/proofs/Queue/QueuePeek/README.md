Assuming xQueue and pvItemToQueue are non-null pointers allocated to the right
size, this harness proves the memory safety of QueueGenericPeek. Some of the
task pool behavior is abstracted away. Nevertheless, some of the concurrent
behavior has been modeled to allow full coverage of QueuePeek.
