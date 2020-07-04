## TimeTracker
A simple C++ 11 class for measuring the execution time of your code block using chrono steady_clock.

It will also calculate the minimum, maximum and average execution time in a given period, which is called report interval.
Every report interval, TimeTracker can generate formatted strings for later usage, or TimeTrack can print the report to standard output.

TimeTracker can also be used to measuing the loop interval, with that, the regualrity of the loop can be visualized, especially for interprocess or inter thread communication.

## Usage

### Declare an TimeTracker object

~~~~~~~~~~cpp
// First parameter is report interval, defaul value is 10 seconds.
// Second parameter is execution count interval, default value is 1 second.
// NOTE: report interval modulo execution count interval, must be 0, otherwise, TimeTracker will force execution count interval to 1.
// The report format will be like the following:

// Exec count [31][30][30][30][30][30][30][30][30][30](301) Exec time(ms) (16.73,16.78,16.84)
// The report line will be printed every 10 seconds
// Each line contains:
//    Exec count: execution count per second, inside []
//    Exec time: execution time in milliseconds, with the order minimum, average, maximum

// A default instance report interval is 10 seconds, execution count interval is 1 second.
TimeTracker myTracker();
// an instance report interval is 60 seconds, execution count interval is 6 seconds.
TimeTracker myTracker(60, 6);
// an instance report interval is 10 seconds, execution count interval is 1 second, with a custom report prefix "MyPerf"        
TimeTracker myTracker(10, 1, "MyPerf");
// an instance report interval is 300 seconds, execution count interval is 60 seconds.
TimeTracker myTracker(300, 60);
// report time unit is microsecond instead of millisecond, support time unit: ms/us/ns
TimeTracker myTracker(10, 1, "Memcpy Perf", "us");
~~~~~~~~~~

## Execution time measuring mode

This might be the most common usage of a timer.

### Using TimeTracker to measure the exection time of the code block

#### Let Timetracker to output report to standard output

```cpp
myTracker.set()

// The code to be meausre the execution time

myTracker.get()

```

#### pass Timetracker a  string stream, and you used it later on.

```cpp
// declare a string stream to store the report result
std::stringstream ss;

myTracker.set()
// The code to be meausre the execution time
if ( myTracker.get(&ss) == true ) {
   std::cout << "MyTimeTracker " << ss.str().c_str() << std::endl;
}

```

## Loop mode

```cpp
for (int i; i< 1000; ++i) {
   myTracker.get()
   // your code
}
```

In this mode, the measure time is the loop interval, no matter the loop is a vector computation or somewhere in the middle of the loop, it is blocked by the semphore or condition variable or or other code make it sleep.


## Example 
