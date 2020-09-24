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

## Quick Start for ESP32

#### Prerequisites

Install [python3](https://www.python.org/downloads/)

Install requirements: `pip install -r requirements.txt`

For available commands and arguments: `./mutation_runner.py -h`

Be able to run the WiFi tests with [ESP32 Getting Started Guide](https://docs.aws.amazon.com/freertos/latest/userguide/getting_started_espressif.html).

#### Try: `./mutation_runner.py start -s wifi -m 3 -c -r`

# Usage

## Prerequisites

Install [python3](https://www.python.org/downloads/)

Install requirements: `pip install -r requirements.txt`

## For available commands and arguments: `./mutation_runner.py -h`

We will now walk through an example for setting up mutation testing for the WiFi library tests on ESP32.

## 1. Set up various requirements for the test group.

In order to run the WiFi tests, we need to configure WiFi credentials and set up an echo server.

Configure your WiFi credentials at `/freertos/tests/include/aws_clientcredential.h`.

Configure echo server address at `/freertos/tests/include/aws_test_tcp.h`. You may use `/freertos/tools/echo_server/` to create an echo server.

Configure a separate set of WiFi credentials at `/freertos/libraries/abstractions/wifi/test/iot_test_wifi.h` for multiple AP connection test.

## 2. Create a config file in `/configs`

See `/configs/wifi.json` as an example.

`task`: List of objects that specify the name of the task, the source files of the task, the test group that will be ran, and the mutation patterns.
- `name`: Name of the task for differentiation purposes.
- `src`: A mapping from the path of the source code file(s) from /freertos to the line numbers to mutate for this file. We can use the coverage tool in the next section to find function coverage and then manually inspect to find the line numbers we want to mutate. If the list is left empty, then all numbers are selected for mutation.
- `test_groups`: The test groups that should be ran to test the source code(s) provided.
- `patterns`: The mutation patterns to use. Choose one from `mutator.py`, or create your own set in that file.
- `mutants_per_pattern` (Optional): The number of mutants to create for each mutation pattern. If not specified then all occurrences of the pattern will be selected as mutants. 

`flash_command`: The `flash_command` attribute is the command executed for cmake, build, compile, and flashing the program onto the board. For ESP32, it is handled by a separate custom python script in this project. Whatever in 'flash_command' will be executed through a python subprocess, so it can be any type of executable script. Make sure to let the script end in a non-exit status code if there issue with compiling or flashing the program as this tells the script that a compile may have failed due to the mutations and advance to the next mutation pattern to try again.

In the future, it is expected that we can generate the this configuration json file with a separate script and the help of a more powerful line coverage tool (see next optional section).

## (Optional) 3. Run the coverage tool to get function coverage for the test.

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

## 4. Begin the mutation testing

Test out the script by running: 

`./mutation_runner.py start -s wifi -m 3`.

The command `start` indicates to start the mutation testing. `-s wifi` pulls the configuration data from `/configs/wifi.json`, and `-m 3` caps the maximum number of mutants to 3.

If the command completes successfully, we should see something like:

```
Successful Mutant Runs: 3/3
Score: 66.67%
Alive: 1 Killed: 2 Mutants: 3 No-Compile: 0 Attempted Runs: 3
Done
```

This means that 3 mutants were created, and 2 failed the test, resulting in a mutation score of 66.67%. There is one mutant that is alive. Let's dive deeper by logging this to CSV. To log to CSV, run the mutation testing again with:

`./mutation_runner.py start -s wifi -m 3 -c`

After the run completes, open up `/CURRENT_DATE/CURRENT_TIME/Full_WiFi_mutants_created.csv`.

*Note that the `expected_catch` section is N/A. This is expected. See step 5.*

Here, we see the details for each mutant created, or something like this:

| line | original                                              | mutant                                                 | result      |
|------|-------------------------------------------------------|--------------------------------------------------------|-------------|
| 566  |                 for (int i = 0; i   < num_aps; i++) { |                 for (int i = 0; i   == num_aps; i++) { | PASS/LIVE   |
| 702  | 		if( pxNetwork->ucSSIDLength < ulSize )                | 		if( pxNetwork->ucSSIDLength == ulSize )                | FAIL/KILLED |
| 715  | 			if( pxNetwork->ucPasswordLength < ulSize )            | 			if( pxNetwork->ucPasswordLength == ulSize )            | FAIL/KILLED |

Now, let's try fixing the test to improve the mutation score. Navigate to the root directory `/freertos/` and open `/freertos/vendors/espressif/boards/esp32/ports/wifi/iot_wifi.c`. Note that line 566 is under the 
`WIFI_Scan` function. We should check the test associated with testing this function.

Open up `/freertos/libraries/abstractions/wifi/test/iot_test_wifi.c` and examine the WiFi_Scan test. Notice that all the test does is verify the return status! Let's improve it.

Comment out the current WIFI_Scan, and add the following:

```C
/**
 * @brief Exercise WIFI_Scan() and verify the return status.
 */
TEST( Quarantine_WiFi, AFQP_WiFiScan )
{
    BaseType_t xNetworkFound = pdFALSE; 

    int32_t lI = 0;
    WIFIScanResult_t xScanResults[ testwifiMAX_SCAN_NUMBER ] = { 0 };
    WIFIReturnCode_t xWiFiStatus;

    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_Scan( xScanResults, testwifiMAX_SCAN_NUMBER );
        TEST_WIFI_ASSERT_EQ_REQUIRED_API( eWiFiSuccess, xWiFiStatus, xWiFiStatus );

        for(lI = 0; lI < testwifiMAX_SCAN_NUMBER; lI++ )
        {
            TEST_ASSERT_NOT_EQUAL( xScanResults[lI].cRSSI, 0 );
            if ( strncmp( xScanResults[lI].cSSID, clientcredentialWIFI_SSID,
                            sizeof( clientcredentialWIFI_SSID ) ) == 0)
                xNetworkFound = pdTRUE;
        }
        TEST_ASSERT(xNetworkFound == pdTRUE);
    }
    else
    {
        TEST_FAIL();
    }
}
```

Now, run `./mutation_runner.py start -s wifi -m 3 -c` again.

We should now see the following:

```bash
Successful Mutant Runs: 3/3
Score: 100.00%
Alive: 0 Killed: 3 Mutants: 3 No-Compile: 0 Attempted Runs: 3
```

We have successfully improved the score to 100%, but, obviously, only creating three mutations is not a comprehensive measure.

## 5. Run the mutation testing with more advanced settings.

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

## (Optional) 6. Run mutation testing with line coverage map.

*This section may require additional tools or manual work to complete.*

You may notice that the `expected_catch` column in `Full_WiFi_mutants_created.csv` is `N/A`. That is because currently the tool is not smart enough to detect which test is expected to catch the a certain mutant. However, we can come to a middle ground. In order to support this feature, this tool has an additional argument `--line_coverage/-l`. Provide the path to a json file with the following format:

```json
{
    "Test1": [
        [87, 165],
        [366, 480]
    ],
    "Test2": [
        [90, 180]
    ]

    ...

    "WiFi_IsConnected": [
        [363,448],
        [87,165],
        [249,263],
        [451,479]
    ]
    "WiFi_Scan": [
        [488,611],
        [87,165]
    ]

    ...
}
```

This map will be used to output which test is supposed to catch a mutant at each any line, if it exists in this map. It will also populate the `expected_catch` column in `Full_WiFi_mutants_created.csv`.

It is useful to know which tests are supposed to catch the mutant when the mutant is **ALIVE** or **passing** the tests. We can then inspect those tests or the source code to see why it's passing.

# Task-based Approach

# Repeating Randomized Tests

# Challenges

## Mutants that do not change behavior of code

Throughout mutation testing, we may notice that some mutations do not actually change the behavior of the code. For example, a mutation pattern may involve changing a `==` into a `>=`. For many cases, like `if xBool == true`, changing it to `if xBool >= true` doesn't change the behavior. When you think about the semantics of many of the operators in different contexts, they are in fact identical. The behavior of the code will not change as a result of the mutant; therefore, it is expected that these such mutants pass the test

It is still difficult for us to identify mutation patterns that do not change the behavior of the code. We can remove it from the pattern set, but at the cost of not being able to identify the mutants with the same mutation pattern that is actually breaking the code but not caught by any tests. To mitigate this issue, this script supports a `task` based approach, which is specified in the configuration file (see [Task-based Approach]()).

## Runtime

Timing Constraint. Max Runtime: The runtime is dependent on the time to run the test (base) and the number of mutants (N) created. eg. O(Base * N). Each mutant takes the full WiFi test suite time = 350 seconds. 350 * 100 mutants = 3500 seconds = ~10 hours, and that is excluding timeout which is currently set to 500 seconds as well as compile failures (takes about 60 seconds to compile each mutant, and fail-to-compile mutants are skipped). Implemented optimization techniques:

- Random selection of mutant. 
- Can also repeat by providing the same random seed. See the jobfile.
- Limiting mutation range to specified set of locations (line numbers).
- Ability to customize own mutant patterns (through configuration json and mutator.py)

## Randomization Issues

This is an unconfirmed issue, but because of randomization, the mutation score between randomized runs that do not retain the same seed will flunctuate greatly. We suspect there is some *averaging* happening, as the scores always converge to around 40-50%. However, between runs with the same seed, if there is an improvement of test quality, the score will definitely be increased.

























