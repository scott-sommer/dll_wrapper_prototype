# dll_wrapper_prototype
An early skeleton of a generic wrapper for numerous dlls which I was integrating into a simulation environment.

The objective of this architecture is to separate concerns into layers for each implementation:

DllModel: Handles dynamic binding to DLLs and any associated setup/teardown. Dynamic binding was used to make it easy to link against what is available for a certain configuration, as one of the requirements was to be able to rapidly compile different configurations, as these DLLs have various classification levels and the system runs in a 'need to know' environment.

BaseModel: Handles any actions/tasks which are common between models. Future concerns which were implemented in production include various things such as coordinate system transformations, etc.

ThreatSystemXXXX: The concrete implementation of a model which is designed using Inversion of Control. All specific details for this ThreatSystem are defined within this single file. This makes it fast to integrate new models (we went from weeks down to hours to integrate new models), and it respects the requirement to be able to rapidly recompile everything with different ThreatSystems included for different classification levels and target environments.

The full implementation in production included a Factory Pattern to instantiate the required models based on the Simulation Configuration, more functionality in the BaseModel layer, as well as some other tweaks to add conveinience functions which handled common clusters of information.