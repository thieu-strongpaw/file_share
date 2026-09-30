# file_share
## Program to share files between nodes on my network.

This is a project to learn network basics and client/server architecture. Security will be the last part added to the system. I hope for the file server to be the launching point for further server development. 

The two componenets of the system are fs_request and fs_server.

### Interface

Accessed from terminal.

The server should be launched from the directory that you wish to server files from. It will try to send any file in that directory over the network, so be cool.

Example for the server:

$>./fs_share

Example for the client: 

$>./fs_request <target_file URL> <destination>



### Issues
- Not in the least bit secure.
- No error handeling past the program crashing out
- Needs optional arguments. At least the ability to rename requested file on client side.
- The README is crap. Who writes this drivel. 
- Need to consider multi-request situations. Can we take advantage of threads?
- create and add some sort of standard for file headers. Something like file name and brief discription at the top...
