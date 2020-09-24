# Mutation Testing for Kernel Library Tests

There are a few tests for the FreeRTOS projects that are sometimes ineffective. An example is this one:

```C
/**    
* @brief Call WIFI_Scan() to scan for zero total networks and verify stability.    
*/    
TEST( Full_WiFi, AFQP_WIFI_Scan_ZeroScanNumber )    
{    
    WIFIScanResult_t xScanResults[ testwifiMAX_SCAN_NUMBER ];    

    if( TEST_PROTECT() )    
    {    
        /* Ports are allowed to decide to return success or failure depending on    
        * their driver for a scan number of zero. */    
        WIFI_Scan( xScanResults, 0 );    
    }    
}
```

The test has since been removed, but the test always passes regardless of what the implementation of WIFI_Scan is because there is no assert statement to fail this test.

To solve or mitigate this issue, mutation testing can be used as a method to improve test quality by introducing one small change to the source code, then running them against the test suite to see if the *mutant* still passes the test. 

Mutants that **PASS** are **ALIVE**, which is bad. Mutants that **FAIL** are **KILLED**, which is good. Whenever the word "failure" is seen in this project, they refer to a mutant failing the tests. This is good because our tests are strong enough that catch the mutant. If they pass the tests, then it is a problem because the mutation of the code is supposed to fail the test.

Each change and it's transformation is called a mutation pattern. Examples of mutation patterns include:

- `==` to `<=`
- `&&` to `||`
- `i++` to `i--`
- `else {` to `\\ else{`
- `return xReturnCode` to `return xReturnCode + 1`

Some features that this script supports are:

- Automating build/flash to generate multiple mutants in succession.
- Ability to specify mutation patterns and source code files as well as lines within files.
- Create random mutants by selecting random pattern and random line in source code.
- Outputs a mutation score as a metric that can be used to see if there is improvement of test quality between mutation runs. The mutation score is a percentage - **FAILS/TOTAL**.
- Generate csvs that show some of the results of the mutation testing.
  
---

## Quick Start

Install [python3](https://www.python.org/downloads/)

Install requirements: `pip install -r requirements.txt`

For available commands and arguments: `./mutation_runner.py -h`

Try: `./mutation_runner.py start -s wifi -m 3 -c -r`

## Usage

### Prerequisites

Install [python3](https://www.python.org/downloads/)

Install requirements: `pip install -r requirements.txt`

### For available commands and arguments: `./mutation_runner.py -h`

We will now walk through an example for setting up mutation testing for the WiFi library tests.

### 1. Set up various requirements for the test group.

In order to run the WiFi tests, we need to configure WiFi credentials and set up an echo server.

Configure your WiFi credentials at `/freertos/tests/include/aws_clientcredential.h`.

Configure echo server address at `/freertos/tests/include/aws_test_tcp.h`. You may use `/freertos/tools/echo_server/` to create an echo server.

Configure a separate set of WiFi credentials at `/freertos/libraries/abstractions/wifi/test/iot_test_wifi.h` for multiple AP connection test.

### 2. Create a config file in `/configs`

See `/configs/wifi.json` as an example.

`task`: List of objects that specify the name of the task, the source files of the task, the test group that will be ran, and the mutation patterns.
- `name`: Name of the task for differentiation purposes.
- `src`: A mapping from the path of the source code file(s) from /freertos to the line numbers to mutate for this file. We can use the coverage tool in the next section to find function coverage and then manually inspect to find the line numbers we want to mutate. If the list is left empty, then all numbers are selected for mutation.
- `test_groups`: The test groups that should be ran to test the source code(s) provided.
- `patterns`: The mutation patterns to use. Choose one from `mutator.py`, or create your own set in that file.
- `mutants_per_pattern` (Optional): The number of mutants to create for each mutation pattern. If not specified then all occurrences of the pattern will be selected as mutants. 

`flash_command`: The `flash_command` attribute is the command executed for cmake, build, compile, and flashing the program onto the board. For ESP32, it is handled by a separate custom python script in this project. Whatever in 'flash_command' will be executed through a python subprocess, so it can be any type of executable script. Make sure to let the script end in a non-exit status code if there issue with compiling or flashing the program as this tells the script that a compile may have failed due to the mutations and advance to the next mutation pattern to try again.

In the future, it is expected that we can generate the this configuration json file with a separate script and the help of a more powerful line coverage tool (see next optional section).

### (Optional) Run the coverage tool to get function coverage for the test.

A function coverage tool is provided if we want to get the functional coverage for the test. All it does is add print statements to the source code files under each function header, and prints out the function name and line number. This is useful if we want to restrict the mutations to a certain set of line numbers in the source code. 

We may also notice that mutants are created in lines that are not executed during the tests. This may lead to inaccurate results as the test passes the mutant, which indicates a potential test quality issue, but in reality, it is an expected behavior. A stronger coverage tool that is able to pinpoint specific lines may be more useful then this tool, but it is yet to be discovered or implemented. `lcov/gcov` was explored, but they do not work when the code is ran on an embedded system board.

To run the function coverage tool: `./mutation_runner.py coverage -s wifi`

We may see the following in the console logs.

```bash
...
---------STARTING TESTS---------
I (7289) wifi:state: run -> init (0)
I (7289) wifi:pm stop, total sleep time: 3633081 us / 4371457 us

I (7289) wifi:new:<11,0>, old:<11,0>, ap:<255,255>, sta:<11,0>, prof:1
I (7289) WIFI: SYSTEM_EVENT_STA_DISCONNECTED: 8
10 699 [RunTests_task] MUTATION_TESTING_FUNC_COVERAGE WIFI_Disconnect: 451
11 699 [eventTask] MUTATION_TESTING_FUNC_COVERAGE event_handler: 87
12 700 [RunTests_task] MUTATION_TESTING_FUNC_COVERAGE WIFI_Scan: 488
...
```

As we can see there are additional `MUTATION_TESTING_FUNC_COVERAGE...` statements which are followed by the function that is being called at this moment in execution as well as the starting line number of this function in the code.

We can then manually inspect the source code to find only the line numbers of the functions that were executed and add them to the `src` section in the configuration json file. This is useful when we want to isolate the a few tests, perhaps the new tests, and only run the mutation testing for these tests. Of course, isolation of the tests involve separating the isolated tests into its own test group, and make sure to provide the test group name to `test_groups` in the configuration json file.

### 3. Begin the mutation testing

Test out the script by running: 

`./mutation_runner.py start -s wifi -m 3`.

The command `start` indicates to start the mutation testing. `-s wifi` pulls the configuration data from `/configs/wifi.json`, and `-m 3` caps the maximum number of mutants to 3.

If the command completes successfully, we should see something like:

```
Successful Mutant Runs: 3/3
Score: 0.00%
Alive: 3 Killed: 0 Mutants: 3 No-Compile: 0 Attempted Runs: 3
Done
```

This means that 3 mutants were created, and all of them successfully passed the tests, resulting in a mutation score of 0%. That doesn't look good. Let's dive deeper by logging this to CSV. To log to CSV, use:

`./mutation_runner.py start -s wifi -m 3 -c`

#### TBC

### 4. Run the mutation testing with more advanced settings.

For additional details on the types of arguments, try: `./mutation_runner.py start -h`.

A typical mutation testing sequence to test the entire WiFi test suite would be:

`./mutation_runner.py start -s wifi -m 100 -r -c`

`-r` randomizes the mutations by selecting a random mutation pattern and a random occurrence of the mutation pattern to mutate for every mutant. 

*Why randomize?* For each mutation pattern, there may be thousands of occurrences depending on the how large the source code is. To save time, but also use a sufficient amount of mutation patterns and lines, it is better to randomize the mutations. 

`-c` logs the results to CSV. Currently, 3 CSV files will be generated for each `task` in the configuration file. They will be logged to `/CURRENT_DATE/CURRENT_TIME/...`.

`Full_WiFi_mutants_created.csv`: Each row has information on the mutated file, line number, original, after, result, and expected tests to catch this mutant.

| file | line | original | mutant | result | expected_catch |
| ---  | ---  | ---      | ---    | ---    | ---            |

`Full_WiFi_pattern_comparison.csv`: Each row has information on the pattern that was used, how many times it failed and the total number of mutants created for each pattern.

| pattern | failures | total | percentage |
| ---     | ---      | ---   | ---        |

`Full_WiFi_test_aggregates.csv`: Each row has information on each specific test and how many times they failed a mutant.

| group | test | kills | passes | total |
| ---   | ---  | ---   | ---    | ---   |

### (Optional) Run mutation testing with line coverage map.

*This section may require additional tools or manual work to complete.*

You may notice that the `expected_catch` column in `Full_WiFi_mutants_created.csv` is `N/A`. That is because currently the tool is not smart enough to detect which test is expected to catch the a certain mutant. However, we can come to a middle ground. In order to support this feature, this tool has an additional argument `--line_coverage/-l`. Provide the path to a json file with the following format:

```json
{
    "1": [
        "Test1",
        "Test2"
    ],
    "2": [
        "Test1",
        "Test3"
    ]

    ...

    "87": [
        "WiFi_IsConnected",
        "WiFi_Scan",
        "WiFi_GetIP"
    ]
    "88": [
        "WiFiConnectionLoop",
        "WiFiOnOffLoop",
        "WiFiMode",
    ]

    ...
}
```

This map will be used to output which test is supposed to catch a mutant at each any line, if it exists in this map. It will also populate the `expected_catch` column in `Full_WiFi_mutants_created.csv`.

It is useful to know which tests are supposed to catch the mutant when the mutant is **ALIVE** or **passing** the tests. We can then inspect those tests or the source code to see why it's passing.

























