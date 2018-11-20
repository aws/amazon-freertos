On Windows

1. Copy Makefile.template to Makefile.local and modify accordingly
2. Build the linked goto program: nmake -FNMakefile
3. Copy the linked goto program to linux: nmake -FNMakefile push

On Linux,

1. Copy Makefile.template to Makefile.local and modify accordingly
2. Launch batch: make launch
3. Copy results: make -f Makefile-ENTRY-TIMESTAMP copy
4. Clean up: make clean launch-clean
5. Clean up everything: make veryclean launch-veryclean
