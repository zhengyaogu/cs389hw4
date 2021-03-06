# CS389 HW4 Let's network
author: Zhengyao Gu, Albert Ji

### Library
We used boost/beast to implement this homework project. The Makefile on this github page is specifically written to work on my(Albert's) laptop, where my boost directory is right under the hw4 directory. You might need to change Makefile to make it work on your machine.

### Server
The server is implemented with a fixed address, port, maxmem and evictor. In order to make the test work, you need to type "make" and turn it background(which is pressiong ctrl+z and type bg on Linux). Then you can type "make Run_client" or "make compile_test" to test the client or run the unit tests.

### Testing
We modified our unit tests in hw3 to fit the new API. Since the server can only run one evictor, you need to specify the evictor that the server is going to use (in the main function of cache_server.cc), "make", turn the server background, and then type "make compile_test".\
Now, type "make xxx_test" to run the unit tests which use xxx evictor. There are three evictors in our project: fifo, lru and null. \
Once the tests are over, you need to shut down the server and manually change the evictor that the server uses. Then run the server background, but you don't need to compile the test again. You can just type "make yyy_test", where yyy is the new evictor that you used.\
Please note that since our server currently runs with a fixed address and a fixed port, it's essential to only have one server running in the background, otherwise you will get many strange errors.\

### Test Result on Our Code
Our project passed all tests.

|Test_case | Test Target | Result|
|--- | --- | --- |
|FIFO | The FIFO evictor | Passed|
|set1 | The set function with no evictor | Passed|
|set2 | The set functino with a FIFO evictor |  Passed|
|get | The get function |  Passed|
|del | The del function | Passed|
|reset | The reset function | Passed|
|space_used | The space_used function | Passed|
|LRU | The LRU evictor | Passed |

### Problems we encountered
There are two subtle problems that are worth talking about. 
 - In the get function of the client, we are supposed to return a char* ptr. However, since client doesn't have the real val saving in its class, we need to create a char arrary on the heap using new and return its ptr. This may cause potential memory leak since the original get function doesn't require the user (and the tests) to delete the return char* to prevent memory leak. However, since our test program terminates, this is not a big issue.
 - The new cache.hh that Eitan gave us doesn't require the del function to return a boolean variable indicating whether the deletion was successful, so our server didn't implement this feature. Hence we slightly modified our unit tests: not to check the result of the deletion directly but to check whether the space_used changes correctly. 
