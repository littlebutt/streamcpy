# streamcpy: The Stream Api in Python

Streamcpy can provide an approach of processing the data in a high-effenciency way. Like in [Java](https://docs.oracle.com/javase/8/docs/api/java/util/stream/Stream.html), you can use stream-like  methods (`map`, `filter`, `for_each` and so on) to do parrallel calculation and lazy calculation, which can accelerating your programs. What's more, The package is implemented in pure C, which means its runtime speed is even higher!

Currently, the methods bellow is implemented.

- [x] filter
- [x] map
- [x] for_each
- [x] collect
- [x] distinct
- [x] limit
- [x] reduce
- [ ] sorted
- [x] max/min
- [x] count
- [ ] any_match/all_match

## Installation

(Not Uploaded yet)

## Usage

Like Stream Api in Java but slightly different in details

```python
from streampy import *

Stream.of([1, 2, 3])
    .map(lambda x: x * x)
    .for_each(lambda x: print(x))

```

This api supports various [Iterable](https://docs.python.org/3/library/stdtypes.html#typeiter) object in Python, like list, tuple and generator. As a result, you can call it in these ways:

```python
from streampy import *

Stream.of((1, 2, 3))
    .map(lambda x: x * x)
    .for_each(lambda x: print(x))

# OR

def gen():
    l = [1, 2, 3]
    for i in l:
        yield i

g = gen()

Stream.of(g)
    .map(lambda x: x * x)
    .for_each(lambda x: print(x))

# OR

Stream.of(open("foo.txt", 'r'))
    .map(lambda x: x * x)
    .for_each(lambda x: print(x))   
```

## Contribution

Welcome to contribute to the project. The project is written in C. If you cannot understand Python's C API, you can refer to the code in __python__ package. It is the streamcpy implemented in python but will not be packed in the package. Here is the detailed description for each directory or file:

- benchmark: a roughly comparison between Python and C++ (*Python wins*)
- build: the built file of the package (`.pyd` for Windows and `.so` for Linux or Mac)
- modules: the source code for streamcpy in C
- python: the source code for streamcpy in python and only for reference
- vs: the project files for Visual Studio
- setup.py: the setup file for python
- streamcpy.c: the entrance file for streamcpy package
- streamcpy.pyi: the `.pyi` file for streamcpy package
  
### Build on Windows

- Download and install the latest [Visual Studio](https://visualstudio.microsoft.com/)
- Install Python Development workload. The workload includes the Python native development tools, which bring in the C++ workload and toolsets that are necessary for native extensions
- Install Desktop Development with C++ workload. It comes with the default core editor, which includes basic code editing support for C/C++
- Double-click the `.vcxproj` in vs folder and build the `.sln` file
- Run the project!

### Build on Linux/Mac

- Download the Python interpreter from the offcial website
- Download the gcc(>7.5.0)
- Run the command below for building
  
```shell
python setup.py build
```

- Run the command below for installing
  
```shell
python setup.py install
```
> NOTE: The method is also suitable for Windows if Visual Studio or Visual Studio Build Tools installed.
