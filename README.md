# streamcpy: The Stream Api in Python

StreamPy can provide an approach of processing the data in a high-effenciency way. Like in Java. you can use stream-like  methods (`map`, `filter`, `for_each` and so on) to do parrallel calculation and lazy calculation, which can accelerating your programs. What's more, The package is implemented in pure C, which means its runtime speed is even higher!

Currently, the methods bellow is implemented.

- [ ] filter
- [x] map
- [x] for_each
- [ ] collect
- [ ] distinct
- [ ] limit
- [ ] reduce
- [ ] sorted
- [ ] max/min
- [ ] count
- [ ] any_match/all_match

## Installation

(Have Not Uploaded yet)

## Usage

Like Stream Api in Java but slightly different in details

```python
from streampy import *

Stream.of([1, 2, 3])
    .map(lambda x: x * X)
    .for_each(lambda x: print(x))

```

You can put any `Iterable` type in `of` method and any `Callable` type in operation methods.

## Contribution

Welcome to contribute to the project. The project is written in C. If you cannot understand Python's C API, you can reference the code in __python__ package. It is the StreamPy implemented in python but will not be packed in the package. Here is the detailed description for each directory or file:

- build: the built file of the package (`.pyd` for Windows and `.so` for Linux or Mac)
- modules: the source code for StreamPy in C
- python: the source code for StreamPy in python and only for reference
- vs: the project files for Visual Studio
- setup.py: the setup file for python
- streampy.c: the entrance file for StreamPy package
  
### Build on Windows



### Build on Linux/Mac