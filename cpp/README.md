# SACCR Exposure Calculator (C++ - Educational)

**Disclaimer:** This repository implements a *simplified, educational* approximation of exposure modelling and a SA‑CCR-like calculation.
It is **not** production-ready and **not** regulatory-compliant. Use at your own risk.

## Build (requires a C++17-compatible compiler and CMake)
```bash
mkdir build && cd build
cmake ..
make -j
./saccr_demo
```

## Features
- Trade types: Interest Rate Swap (IRS), FX Forward, European Option (Black-Scholes)
- Monte Carlo exposure profiles (simplified): Expected Positive Exposure (EPE), PFE (95% quantile)
- Simplified SA‑CCR-style EAD: `EAD = alpha * (RC + PFE_addon)` with toy supervisory factors

This is intentionally lightweight and self-contained: no external dependencies.
