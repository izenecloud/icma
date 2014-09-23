Chinese Morphological Analyzer
=============================================

### Features
The technical report for the analyzer could be seen [here](https://github.com/izenecloud/icma/raw/master/docs/pdf/chinese-ma-tr.pdf).


### Dependencies
We've just switched to `C++ 11` for SF1R recently, and `GCC 4.8` is required to build SF1R correspondingly. We do not recommend to use Ubuntu for project building due to the nested references among lots of libraries. CentOS / Redhat / Gentoo / CoreOS are preferred platform. You also need `CMake` and `Boost 1.56` to build the repository . Here are the dependent repositories list:

* __[cmake](https://github.com/izenecloud/cmake)__: The cmake modules required to build all iZENECloud C++ projects.


### License
The project is published under the Apache License, Version 2.0:
http://www.apache.org/licenses/LICENSE-2.0
