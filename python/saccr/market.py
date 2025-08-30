from dataclasses import dataclass
import numpy as np

@dataclass
class MarketParams:
    r0: float = 0.03
    sigma_r: float = 0.01
    mu_fx: float = 0.00
    sigma_fx: float = 0.10
    mu_eq: float = 0.05
    sigma_eq: float = 0.20
    fx0: float = 1.25
    s0: float = 100.0
    discount_rate: float = 0.03

def time_grid_years(T: float, steps: int):
    t = np.linspace(0.0, T, steps + 1)
    dt = np.diff(t, prepend=t[0])
    dt[0] = 0.0
    return t, dt
