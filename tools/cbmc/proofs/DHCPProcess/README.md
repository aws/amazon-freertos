This proof demonstrates the memory safety of the DHCPProcess function, which is
the function that triggers the DHCP protocol. The main stubs in this proof deal
with buffer management, which assume that the buffer is large enough to
accomodate a DHCP message plus a few additional bytes for options (which is the
last, variable-sized field in a DHCP message). We have abstracted away sockets,
concurrency and third-party code. For more details, please check the comments
on the harness file.
