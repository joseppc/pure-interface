# pure-interface

### This project experiments software technologies to:

* support multiple implementations of a set of APIs in a modular framework
* build implementations as static libraries or DSOs (loadable modules)
* statically link implementation with no performance penalty
* dynamically load implementation with API pointers

### Abstractions:

* The software can be partitioned into multiple subsystems
* Each subsystem represents a view of the component and provides APIs to
  manipulate the component in that 'view'.
  * Example: scheduler is the component, we can:
  * Create subsystem(scheduler) to provide public APIs, and
  * Create subsystem(scheduler_internal) to provide accelerated internal access APIs.
* Each subsystem can have multiple implementations.
* Subsystem implementations are written as modules.
* Implementation modules can be built into static libraries as well as DSOs.
* Subsystem API stubs can route the call to the selected implementation or
  iterate all registered implementation modules.
* Provide facilities for implementation modules to override subsytem API stubs
  to remove all the overheads (locks and pointers).
