## Structure

```text
.
├── src/                    # C++ source files
│   ├── main.cpp
│   ├── montecarlo.cpp
│   ├── saccr.cpp
│   └── ...
├── include/                # C++ headers
│   ├── montecarlo.hpp
│   ├── saccr.hpp
│   └── trades.hpp
├── saccr/                  # Python package
│   ├── market.py
│   ├── montecarlo.py
│   ├── trades.py
│   ├── portfolio.py
│   ├── saccr.py
│   └── cli.py
├── tests/                  # Python tests
│   └── test_saccr.py
└── CMakeLists.txt
```