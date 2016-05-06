# message transport
Draft of a library that implements a message transport for interprocess dataflow applications 

Code is based on ethzasl_message_transport (Maintained by Cedric Pradalier) library.
The refactoring focuses on creating a standalone, reusable message transport library that is suitable for image streaming with a focus on shared-memory communication and eventually a fallback to network based message exchange.

The motivation behind this work is to separate hardware drivers from the actual dataflow-based processing.
Such a setup would have the following benefits:

 - On windows platforms it would remove constraints for the main applications based on the compiler requirements of hardware drivers/sdks
 - Programs could be restarted without starting/stopping device communication
 - If any of the components crashes (e.g. hardware drivers) it could be relaunched without affecting the running system (much)..
 
 Be aware - this is a playground - not even alpha software or anything near usable - use at your own risk !!
 
 Ulrich Eck
