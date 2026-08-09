# C++26 Dependency Inection

This is an attempt at making a dependency injector using reflection.

## TODO

  - [X] create object in get<>()
  - [X] support creating arbitrary objects with inject<> properties from container
  - [X] support arguments in builder.add_*() to pass into ctor of T in get<T>()
  - [X] abstract the interface so creation of di container can be done in one TU, but consumed in another without full types
  - [X] add support for singletons
  - [ ] add support for scope based objects
  - [ ] add support for transient objects
  - [ ] add support for chaining containers
  - [ ] add support for injection of "interfaces" for concrete types (add_singleton<TInterface, TConcrete>(), etc)
  - [ ] scope destruction in reverse topological order of dependency to ensure safe destruction
  - [ ] dependency cycle detection
  - [ ] allocator support
  - [ ] pin scope to thread-local or user defined context

Initial working prototype. However, managing scopes, transients and combining di containers is a way off.

Also need to think about how to hide implementation details / types behind an "interface" so that the 
di consumers don't have to have the full definitions of the container and all entities visible, only
what they need.

## Example usage

See the `src` folder for a simple example.

Note that we can firewall off the knowledge of the full type definitions being injected to a single translation
unit building the `di::container` and to the TU for any methods referenceing the injected objects. 

* **main.cpp** - gets the `di::provider` type erased interface from the `registration.cpp` TU, then instantiates 
  a `calculator` and uses it to generate a result
* **registration.hpp** defines a function to return the type erased contain interface (a `di::provider`).
* **registration.cpp** builds the `di::container`. Needs to know the full definition of its registered types
* **calculator.hpp** defines the calculator service and its required injected types. Note they are only forward
  declared, but could also be fully declared here
* **calculator.cpp** implements the `calculator::value()` method. This needs the full definition of the injected
  constants.
* **const_{1,2}.hpp** simple services to add to the `di::container` to provide constant values used by the `calculator`

This is a pretty contrived example, but shows the features of this library.

