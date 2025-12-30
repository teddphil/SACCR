# SA-CCR Exposure & EAD Analytics (C++ / Python)

A **counterparty credit risk analytics prototype** that simulates **future exposure profiles** for interest rate, FX, and equity derivatives using Monte Carlo methods, and computes **SA-CCR-style Exposure at Default (EAD)** using supervisory add-on logic.

This project is designed to demonstrate the **end-to-end workflow** used in front-office and XVA teams:
**market simulation → trade revaluation → exposure aggregation (EPE/PFE) → regulatory-style EAD computation**.

---

## Key Capabilities

### Exposure Simulation
- Monte Carlo simulation of future market states
- Pathwise revaluation of derivative contracts
- Exposure calculation under positive exposure conventions

### Supported Instruments
- **Interest Rate Swap**
  - Linearized PV via DV01 under flat-rate assumptions
- **FX Forward**
  - Discounted forward valuation with domestic/foreign rates
- **European Option**
  - Black–Scholes valuation for equity or FX underlyings

### Exposure Metrics
- **EPE (Expected Positive Exposure)**
- **PFE (Potential Future Exposure)** at configurable confidence levels

### SA-CCR Analytics (Reference Implementation)
- Replacement Cost: `RC = max(MtM − collateral, 0)`
- Supervisory add-on aggregation across asset classes
- `EAD = α × (RC + PFE_addon)` with configurable supervisory parameters

---

## Dual-Language Implementation

### C++
- Performance-oriented Monte Carlo engine
- Single executable demonstrating exposure profiles and EAD calculation
- Clear separation of market, trade, simulation, and SA-CCR logic
- Built with modern C++ (C++17)

### Python
- Research-oriented API for rapid experimentation
- Command-line interface for running example portfolios
- Unit tests validating simulation and exposure mechanics
- Optional plotting of exposure profiles

---

## Repository Structure

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
