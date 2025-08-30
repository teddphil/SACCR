# SACCR Exposure Calculator (Educational)

**Disclaimer:** This repository implements a *simplified, educational* approximation of exposure modelling and a SA‑CCR-like calculation.
It is **not** production-ready and **not** regulatory-compliant. Use at your own risk.

## Features
- Trade types: **Interest Rate Swap (IRS)**, **FX Forward**, **European Option** (on equity or FX).
- **Monte Carlo** exposure profiles: Expected Positive Exposure (EPE), Potential Future Exposure (PFE).
- **Simplified SA‑CCR-style** EAD: `EAD = alpha * (RC + PFE_addon)` with toy supervisory factors.
- Netting set aggregation.
- Minimal **CLI** and **unit tests**.

## Quickstart

```bash
pip install -r requirements.txt
python -m saccr.cli --example         # runs an example portfolio
python -m saccr.cli --plot            # plots exposure profile
```

You can also run the example script:
```bash
python examples/example_portfolio.py
```

## Structure
```
saccr_exposure_calculator/
├── saccr/
│   ├── __init__.py
│   ├── market.py
│   ├── trades.py
│   ├── montecarlo.py
│   ├── portfolio.py
│   ├── saccr.py
│   └── cli.py
├── examples/
│   └── example_portfolio.py
├── tests/
│   └── test_saccr.py
├── requirements.txt
└── README.md
```

## Notes
- Interest rates are simulated with a simple **Gaussian** model; FX/Equity with **GBM**.
- Swap PV uses a **DV01-linear** approximation for speed.
- European option priced with **Black–Scholes** on each path for exposure.
- Parameters and factors in `saccr/saccr.py` are placeholders for demonstration.

## License
MIT
