# RIPv2_Cpp_localhostNetwork
RIPv2 implementation. Run multiple programs on single system using config files passed by CLI argument

Instructions:
Inside the cmake-build-debug folder is the executable and seven configuration files.
  1. Download the executable and config files or create your own
  2. Create a directory and place the executable and config files in it.
  3. Using command line navigate to the directory and launch the executable using argument config<x>.ini (e.g. "./RIP_ config1.ini)"
  4. To launch multiple "routers" use another console window and run other config file.

The program periodically print out the current routing table, messages when it sends an update, receives an update and what routes
are updated. This is not the full implementation of RIPv2, triggered updates only occur when a route is declared "dead" and not 
everytime a better route is discovered. Also, no authorizations etc.
