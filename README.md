# mpDNS
A DNS server written in C. 
The backend is a MySQL database. The configuration GUI is web-based and written in PHP (lol).


I wrote the DNS Server 2002 for my state exam. It was the practical part of the exam.

The source code is unchanged. 
The inline documentation is in German, and it even includes the passwords for the test setup ;)

On modern systems this will not compile. Too much stuff is odd ;)

For example:
- I used the filename "error.h" for a file which is normaly a system include.
- The POSIX defines are odd. So some "implicit declaration of function" pop up during compile.
- ...

A good example of how time can render a codebase unusable.

# Known errors
This version may have an error with PTR requests ;)

# License
License is GPL v3.0
