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
Currently all known bugs are fixed and our projects passed all the tests on my machine.
