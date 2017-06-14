# pure-interface

### This project experiments software technologies to support:

* Multiple implementations of software components in a modular framework
* Build implementations as static libraries or DSOs (loadable modules)
* Software component to route API calls to either implementation in runtime
* Override APIs with specific implementation in both static and DSO builds

### Abstractions: (to write a software library)

* The software library can be partitioned into multiple subsystems
* Each subsystem represents a view of the component and provides APIs to
  manipulate the component in that 'view'
  * Example: scheduler is the component, we can:
  * Create subsystem(scheduler) to provide public APIs, and
  * Create subsystem(scheduler_internal) to provide accelerated internal access APIs
* Each subsystem can have multiple implementations, subsystem implementations
  are written as modules
  * Modules can be built into static libraries or DSOs (loadable modules)
* Subsystem APIs can route the call to either implementation modules in runtime
  * This API route has overheads as read locks and pointer references
  * Modules built in static libraries or DSOs can override subsystem APIs

### Sample: pure-interface software library

* Two subsystems (both have two APIs):
  * Scheduler has one implementation: scheduler-default
  * Pktio has two implementations: pktio-loop and pktio-socket

* Build targets (to demonstrate):
  * main (with DSOs)
    * The dynamic module loader loads modules (libscheduler-default.so,)
      and the subsystem API stubs route the calls to the loaded modules
  * main-static
    * The static built modules (libscheduler-default.a,) are compiled in
      and the subsystem API stubs can route the call to the static modules.
  * main-static-overload
    * Scheduler API stubs can be overridden by static implementation module
      (libscheduler-default-override.a)
  * libscheduler-default-override.so
    * run command "$LD_PRELOAD=./libscheduler-default-override.so main" to
      observe that scheduler API stubs can be overridden by DSO implementation
      module.