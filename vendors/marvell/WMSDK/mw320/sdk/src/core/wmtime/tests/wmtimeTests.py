import unittest
import pexpect
import time

global config
global mtfterm

class TestWMTIME(unittest.TestCase):

    def setUp(self):
        mtfterm.sendline("Module wmtime registered");
        mtfterm.getprompt();

    #Comparison function that compares two time values.
    def compareNormalTime(self, expectedVal, line):
        originalExpect = expectedVal;
        originalLine = line;
        line = line.rsplit(':', 1)
        expectedVal = expectedVal.rsplit(':', 1);
        if (line[0] == expectedVal[0]) and (int(line[1]) >= int(expectedVal[1])) and (int(line[1]) <= int(expectedVal[1]) + 3):
		return 0;
        else:
		print ("Fail: expected:" + str(originalExpect) + " actual:" + str(originalLine));
		return 1;

    #Comparison function that compares two posix values.
    def comparePosixTime(self, expectedVal, actualVal):
        if (actualVal >= expectedVal) and (actualVal < expectedVal + 2):
		return 0;
        else:
		print ("Fail: expected:" + str(expectedVal) + " actual:" + str(actualVal));
		return 1;

    #Run test for time set/get for normal time.
    def runTestSetGet(self, setVal, expectedSetVal, expectedGetVal):
        mtfterm.sendline(("time-set " + setVal));
        line = mtfterm.readline();
        line = mtfterm.readline();
        if ((self.compareNormalTime (expectedSetVal, line)) == 0):  #if set is successful
            time.sleep(3);
            mtfterm.sendline("time-get");
            line = mtfterm.readline();
            return self.compareNormalTime (expectedGetVal, line); #status of get
        else:
            return 1;

    #This is a positive test which sets different year, month, day and time combination for "time-set" command and checks for the appropriate result.
    def testSetGetTime(self):
	testFail = 0;

	#Updating Sec:
        setVal = "2011 9 28 3 40 20";
        expectedSetVal = "Wed 2011 Sep 28 03:40:20";
        expectedGetVal = "Wed 2011 Sep 28 03:40:23";
	if self.runTestSetGet(setVal, expectedSetVal, expectedGetVal) > 0:
		testFail = testFail +  1;
		print "Failed in updating second";

	#Updating Minute:
        setVal = "2011 9 28 3 40 59";
        expectedSetVal = "Wed 2011 Sep 28 03:40:59";
        expectedGetVal = "Wed 2011 Sep 28 03:41:02";
	if self.runTestSetGet(setVal, expectedSetVal, expectedGetVal) > 0:
		testFail = testFail +  1;
		print "Failed in updating minute";

	#Updating Hour:
        setVal = "2011 9 28 3 59 59";
        expectedSetVal = "Wed 2011 Sep 28 03:59:59";
        expectedGetVal = "Wed 2011 Sep 28 04:00:02";
	if self.runTestSetGet(setVal, expectedSetVal, expectedGetVal) > 0:
		testFail = testFail +  1;
		print "Failed in updating hour";

	#Updating Day:
        setVal = "2011 9 28 23 59 59";
        expectedSetVal = "Wed 2011 Sep 28 23:59:59";
        expectedGetVal = "Thu 2011 Sep 29 00:00:02";
	if self.runTestSetGet(setVal, expectedSetVal, expectedGetVal) > 0:
		testFail = testFail +  1;
		print "Failed in updating day";

	#Updating Month:
        setVal = "2011 9 30 23 59 59";
        expectedSetVal = "Fri 2011 Sep 30 23:59:59";
        expectedGetVal = "Sat 2011 Oct 01 00:00:02";
	if self.runTestSetGet(setVal, expectedSetVal, expectedGetVal) > 0:
		testFail = testFail +  1;
		print "Failed in updating month";

        #Updating LeapYear:
        setVal = "2008 2 29 23 59 59";
        expectedSetVal = "Fri 2008 Feb 29 23:59:59"
        expectedGetVal = "Sat 2008 Mar 01 00:00:02";
	if self.runTestSetGet(setVal, expectedSetVal, expectedGetVal) > 0:
		testFail = testFail +  1;
		print "Failed in updating leap year";

	#Updating Year:
        setVal = "2011 12 31 23 59 59";
        expectedSetVal = "Sat 2011 Dec 31 23:59:59";
        expectedGetVal = "Sun 2012 Jan 01 00:00:02";
	if self.runTestSetGet(setVal, expectedSetVal, expectedGetVal) > 0:
		testFail = testFail +  1;
		print "Failed in updating year";

	self.failIf(testFail > 0, "Failed in testSetGetTime");

    #This is positive test which sets posix value of date and gets the posix value and compares this value with expected value.
    def testSetGetTimePosix(self):
	testFail = 0;
        expectedVal = 1317186263;
        mtfterm.sendline(("time-set-posix " + str(1317186260)));
        time.sleep(3);
	mtfterm.sendline("time-get-posix");
        line = mtfterm.readline();
        line = int(line);
	testFail = testFail + self.comparePosixTime(expectedVal, line);
	self.failIf(testFail > 0, "Failed in testSetGetTimePosix");

    #This is positive test which sets the date using time-set command and gets the value using time-get-posix command and checks the value with expected value.
    def testSetTimeGetPosixTime(self):
	testFail = 0;
        setVal = "2011 9 28 5 4 20";
        expectedVal = 1317186263;
        mtfterm.sendline(("time-set " + setVal));
	time.sleep(3);
	mtfterm.sendline("time-get-posix");
        line = mtfterm.readline();
        line = int(line);
	testFail = testFail + self.comparePosixTime(expectedVal, line);
	self.failIf(testFail > 0, "Failed in testSetTimeGetPosixTime");

    #This is positive test which sets the posix time using time-set-posix and gets the date value using time-get command and checks the value with expected value.
    def testSetPosixTimeGetTime(self):
	testFail = 0;
        posixVal = 1317186260;
        expectedVal = "Wed 2011 Sep 28 05:04:23";
        mtfterm.sendline(("time-set-posix " + str(posixVal)));
        time.sleep(3);
        line = mtfterm.sendline("time-get");
        line = mtfterm.readline();
	testFail = testFail + self.compareNormalTime(expectedVal, line);
	self.failIf(testFail > 0, "testSetPosixTimeGetTime");

    # This test includes some negative tests for validation.
    def testValidationTest(self):
	testFail = 0;
	errorMsg = "Usage: time-set <year> <month> <day> <hour> <min> <sec> (all numeric)";

	#Month more then 12.
	setVal = "2009 13 12 3 40 12";
        mtfterm.sendline(("time-set " + setVal));
	line = mtfterm.readline();
	if line != errorMsg:
		testFail = testFail + 1;
		print ("\nFailed in Month more then 12");

	#Day is more then 31.
	setVal="2009 12 32 3 40 12";
        mtfterm.sendline(("time-set " + setVal));
	line = mtfterm.readline();
	if line != errorMsg:
		testFair = testFail + 1;
		print ("\nFailed in Day is more then 31");

	#Pass Hours as negative value.
	setVal = "2011 11 30 -1 4 12";
        mtfterm.sendline(("time-set " + setVal));
	line = mtfterm.readline();
	if line != errorMsg:
		testFail = testFail + 1;
		print ("\nFailed in Hours as negative value");

	#Pass Minutes as negative value.
	setVal = "2011 11 30 1 -1 12";
        mtfterm.sendline(("time-set " + setVal));
	line = mtfterm.readline();
	if line != errorMsg:
		testFail = testFail + 1;
		print ("\nFailed in  Minute as negative value");

	#Insufficient arguments.
	setVal = "2009 11 31 4 12";
        mtfterm.sendline(("time-set " + setVal));
	line = mtfterm.readline();
	if line != errorMsg:
		testFail = testFail + 1;
		print ("\nFailed in Insufficient arguments");

	#Set date as 31 in Sep Month.
	setVal = "2011 11 31 22 00 00";
        mtfterm.sendline(("time-set " + setVal));
	line = mtfterm.readline();
	line = line.split('.', 1);
	if line[0] != "Error: Validation failed":
		testFail = testFail + 1;
		print ("\nFailed to set date as 31 in September month");
	#Pass hours as 25.
        setVal = "2011 11 30 25 12 12";
        mtfterm.sendline(("time-set " + setVal));
        line = mtfterm.readline();
        if line != errorMsg:
                testFail = testFail + 1;
                print ("\nFailed in Hour as 25");

        #Pass minutes as 60.
        setVal = "2011 11 3 3 60 12";
        mtfterm.sendline(("time-set " + setVal));
        line = mtfterm.readline();
        if line != errorMsg:
                testFail = testFail + 1;
                print ("\nFailed in Minutes as 60");

	#Pass negative value in Posix time.
        mtfterm.sendline(("time-set-posix " + str(-123)));
	line = mtfterm.readline();
	#line = "Usage: time-set <year> <month> <day> <hour> <min> <sec> (all numeric)";
	if line != "Usage: time-set-posix <sec_since_epoch>":
		testFail = testFail + 1;
		print ("\nFailed in negative value in Posix time");

	self.failIf(testFail > 0, "Failed in testValidationTest" );
