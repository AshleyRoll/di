# C++26 Dependency Inection

This is an attempt at making a dependency injector using reflection.

## TODO

  - [X] create object in get<>()
  - [X] support creating arbitrary objects with inject<> properties from container
  - [X] support arguments in builder.add_*() to pass into ctor of T in get<T>()
  - [ ] abstract the interface so creation of di container can be done in one TU, but consumed in another without full types
  - [ ] add support for singletons
  - [ ] add support for scopes
  - [ ] add support for chaining containers
  - [ ] add support for injecting a container/scope into a class

Initial working prototype. However, managing scopes, transients and combining di containers is a way off.

Also need to think about how to hide implementation details / types behind an "interface" so that the 
di consumers don't have to have the full definitions of the container and all entities visible, only
what they need.

## Thoughts

- type erased interface for requesting object using a some sort of global 
  lookup. The lookup function is set by the current di. 
- How to deal with scopes and tracking that thought threads/async operations


