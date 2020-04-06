# CS389 HW4 Let's network
author: Zhengyao Gu, Albert Ji

### Library
We used boost/beast to implement this homework project. The Makefile on this github page is specifically written to work on my(Albert's) laptop, where my boost directory is right under the hw4 directory. You might need to change Makefile to make it work on your machine.

### Server
The server is implemented with a fixed address, port, maxmem and evictor. In order to make the test work, you need to type "make Run_server" and turn it background(which is pressiong ctrl+z and type bg on Linux). Then you can type "make Run_client" or "make" to test the client and the server.

### Testing
Unfortunately, since the server is currently implemented in a fixed maxmem and evictor, most of our hw3 unit tests don't work anymore. However, we did test the server and client indepentently, along with a remaining hw3 unit testing. Currently all known bugs are fixed.
