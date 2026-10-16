# C++26 Dependency Injection

This is an attempt at making a dependency injector using reflection.

## TODO

- [X] create object in get<>()
- [X] support creating arbitrary objects with inject<> properties from container
- [X] support arguments in builder.add_*() to pass into ctor of T in get<T>()
- [X] abstract the interface so registration of types in the di container can be done in one TU, but consumed in another
  without full information visible
- [X] add support for singletons
- [ ] add support for scope based objects
- [X] add support for transient objects
- [ ] thread safety
- [ ] add support for injection of "interfaces" for concrete types (add_singleton<TInterface, TConcrete>(), etc)
- [ ] dependency cycle detection
- [ ] allocator support
- [ ] pin scope to thread-local or user defined context

## Example usage

See the `src` folder for a simple example.

Note that we can firewall off the knowledge of the full type definitions being injected to a single translation
unit building the `di::container` and to the TU for any methods referencing the injected objects.

* **main.cpp** - creates the `di::builder` and passes to the `registration.cpp` TU to add types, then instantiates
  a `calculator` and uses it to generate a result. It also registers local types and builds the final `di::provider`
* **registration.hpp** defines a function to register calculator types in the `di::builder`
* **registration.cpp** registers the calculator types in the `di::builder`. Needs to know the full definition of
  its registered types.
* **calculator.hpp** defines the calculator service and its required injected types. Note they are only forward
  declared, but could also be fully declared here
* **calculator.cpp** implements the `calculator::value()` method. This needs the full definition of the injected
  constants.
* **const_{1,2}.hpp** simple services required by the `calculator` to fetch constants as a example of injected
  dependencies

This is a pretty contrived example, but shows the features of this library.

