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
- file_share_remote sends response back: {ok, bad}
- file_share_remote sends size of file
- file_share_local creates buffer for file
- file_share_remote sends data
- file_share_local saves file to disk
