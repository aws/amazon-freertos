# Overview
This is about how to use Doxygen to maintain the API documents. There are three parts:

- Doxygen configuration and layout.
- A separate file to structure the documents
- Comment properly in source code with Doxygen special commands

# Setup Doxygen

[Doxygen Manual](http://www.doxygen.nl/manual/index.html)

- Download and install Doxygen
- Install graphviz
	- on macOS, run "brew install graphviz"
	- on Linux, run "sudo apt install graphviz"
- Go to the root directory of "amazon-freertos" (this will be referenced as $ROOT_AFR_DIR)
	- cd $ROOT_AFR_DIR
- Run generate_doc.sh to generate the documents. All warnings will go to $ROOT_AFR_DIR/doxygen_warnings.txt. 
  ```
  $ ./doc/generate_doc.sh .
  ```
  This script runs on macOS, Linux, and Windows Subsystem for Linux (WSL).
- The entry document is $ROOT_AFR_DIR/doc/output/main/index.html, open it with a browser and verify that it looks good.

# Add documents for a new library

**Let's say the library name is "Foo"**

## 1. Doxygen configuration and layout

### create a new configuration file for "Foo"

The new configuration's path is $ROOT_AFR_DIR/doc/config/foo

Copy below as content:
- [REQUIRED] update the library name accordingly (anywhere appear as "foo" below)
- [REQUIRED] update "INPUT" configuration
- [REQUIRED] update "EXAMPLE_PATH" configuraton
- [OPTIONAL] update "FILE_PATTERNS" configuration if new file extension is introduced



```
# Include common configuration options.
@INCLUDE_PATH = doc/config
@INCLUDE = common

# Basic project information.
PROJECT_NAME = "Foo"

# Library documentation output directory.
HTML_OUTPUT = foo

# Generate Doxygen tag file for this library.
GENERATE_TAGFILE = doc/output/foo.tag

# Directories containing library source code.
INPUT = [The source files path]

# Library file names.
FILE_PATTERNS = *.h *.c *.txt

EXAMPLE_PATH = [The source files path]

# External tag files required by this library.
TAGFILES = doc/output/main.tag=../main \
```

## 2. A separate file to structure the documents

The path to put this txt file: $ROOT_AFR_DIR/doc/lib/foo.txt

In the following sections, follow each of them and **append** the content to the txt file.

### denote mainpage

```
/**
@mainpage
*/
```

### add configuration page

TODO: this section is copy-pasted currently. Need a better way.

```
/**
@config_page{Foo}
@config_brief{library}

@section ......
@brief ......

......

@configpossible ......
@configrecommended ......
@configdefault ......
*/
```

### add constant page

```
/**
@constants_page{Foo}
@constants_brief{library}

@section foo_constants_single Single Value Constants
- @ref ......

@section foo_constants_multiple Multiple Values Constants
- @ref ......
*/
```

### add function page

```
/**
@functions_page{foo, Foo}
@functions_brief{foo}
- @function_name{foo_function_socket}
- @function_brief{foo_function_socket}
*/

/**
@page foo_function_func FOO_func
@snippet aws_foo.h declare_foo_func
@copydoc FOO_func"
*/
```

### add data type page

```
/**
@handles_group{Foo}
@handles_brief{library}

@paramstructs_group{Foo}
@paramstructs_brief{Foo, Foo library}
*/
```

## 3. Comment properly in source code with Doxygen special commands

### add @[] declaration block to functions

```
/* @[declare_foo_func] */
void FOO_func();
/* @[declare_foo_func] */
```

### add @ingroup to data types 

```
@ingroup Foo_datatypes_handles
```

```
@ingroup Foo_datatypes_enums
```

```
@ingroup Foo_datatypes_paramstructs
```

### add @anchor and @name to group of constants

```
/**
 * @anchor AGroupOfConstants
 * @name AGroupOfConstants
 * ...
 */

/**@{ */

 #define Constants_1
 #define Constants_2  

/**@} */

```


