# file_share
## Program to share files between nodes on my network.

### Interface

Accessed from terminal

Example: $>file_share <target_file URL> <destination>

### Program Flow

- User enters the URL for the file and the destination for the file
- file_share_local pareses user input. 
- file_share_local creates request for file
- file_share_local sends request over TCP/IP
- file_share_remote locates file
- file_share_remote sends size of file
- file_share_local creates buffer for file
- file_share_remote sends data
- file_share_local saves file to disk


### Issues
- Not in the least bit secure.
- No error handeling past the program crashing out
- Needs optional arguments. At least the ability to rename requested file on client side.
- The README is crap. Who writes this drivel. 
- Need to consider multi-request situations. Can we take advantage of threads?
- There is no need for the Queue.h. Remove this.
- create and add some sort of standard for file headers. Something like file name and brief discription at the top...
