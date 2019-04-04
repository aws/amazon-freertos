CBMC Proof Infrastructure
=========================

This directory contains automated proofs of the memory safety of various parts
of the amazon:FreeRTOS codebase. A continuous integration system validates every
pull request posted to the repository against these proofs, and developers can
also run the proofs on their local machines.

The proofs are checked using the [C Bounded Model
Checker](http://www.cprover.org/cbmc/), an open-source static analysis tool
([GitHub repository](https://github.com/diffblue/cbmc)). This README describes
how to run the proofs on your local clone of a:FR.


Prerequisites
-------------

You will need Python 3. On Windows, you will need Visual Studio 2015 or later
(in particular, you will need the Developer Command Prompt and NMake). On macOS
and Linux, you will need Make.


Installing CBMC
---------------

- Clone the [CBMC repository](https://github.com/diffblue/cbmc).
- Follow the installation instructions for your platform in CBMC's
  [COMPILING.md](https://github.com/diffblue/cbmc/blob/develop/COMPILING.md)
  file.

  - For Windows users, we recommend building CBMC using CMake in a Visual Studio
    Developer Command Prompt. There is no need to build CBMC with Java support
    to run the a:FR proofs, so you may pass `-DWITH_JBMC=OFF` to CMake's
    configure stage; this avoids the need to install Java dependencies on your
    machine. We recommend that you configure for a release build of CBMC by
    passing `-DCMAKE_BUILD_TYPE=Release` to CMake's configure stage.

  - Linux and macOS users can use the CMake or Makefile build; we recommend
    CMake for ease of installation. Again, we recommend passing
    `-DWITH_JBMC=OFF` and `-DCMAKE_BUILD_TYPE=Release` if using CMake.

- Ensure that you can run the programs `cbmc`, `goto-cc` (or `goto-cl` on
  Windows), and `goto-instrument` from the command line. If you build CBMC with
  CMake, the programs will have been installed under the `build/bin` directory
  under the top-level `cbmc` directory; you should add that directory to your
  `$PATH`. If you built CBMC using Make, then those programs will have been
  installed in the `src/cbmc`, `src/goto-cc`, and `src/goto-instrument`
  directories respectively.


Setting up the proofs
---------------------

Change into the `proofs` directory. On Windows, run
```
python make-common-makefile.py
python make-proof-makefiles.py
```
On macOS or Linux, run
```
./make-common-makefile.py
./make-proof-makefiles.py
```
If you are on a Windows machine but want to generate Linux Makefiles (or vice
versa), you can pass the `--system linux` or `--system windows` options to those
programs.


Running the proofs
------------------

Each of the leaf directories under `proofs` is a proof of the memory
safety of a single entry point in a:FR. The scripts that you ran in the
previous step will have left a Makefile in each of those directories. To
run a proof, change into the directory for that proof and run `nmake` on
Windows or `make` on Linux or macOS. The proofs may take some time to
run; they eventually write their output to `cbmc.txt`, which should have
the text `VERIFICATION SUCCESSFUL` at the end.


Proof directory structure
-------------------------

This directory contains the following subdirectories:

- `proofs` contains the proofs run against each pull request
- `patches` contains a set of patches that get applied to the codebase prior to
  running the proofs
- `include` and `windows` contain header files used by the proofs.
