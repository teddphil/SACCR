from __future__ import annotations
from dataclasses import dataclass
import numpy as np
from typing import Literal

@dataclass
class IRSwap:
    notional: float
    maturity: float       # in years
    fixed_rate: float     # fixed leg rate
    pay_fixed: bool       # True: pay fixed, receive float
    dv01_bp: float = None # DV01 in per 1bp PV change (currency units per bp)

    def pv(self, r: float) -> float:
        """
        Toy PV using flat-rate discounting and DV01 linearization:
        PV ≈ DV01 * 10000 * (r - fixed_rate) if pay_fixed else negative of that sign.
        """
        if self.dv01_bp is None:
            # approx DV01 ~ notional * maturity * discount factor / 10000
            df = np.exp(-r * self.maturity)
            self.dv01_bp = self.notional * self.maturity * df / 10000.0
        sign = 1.0 if self.pay_fixed else -1.0
        return sign * self.dv01_bp * 10000.0 * (r - self.fixed_rate)

@dataclass
class FXForward:
    notional_base: float
    maturity: float
    strike: float         # forward rate (price in quote/spot terms)
    direction: Literal["buy_base","sell_base"]

    def pv(self, fx: float, r_d: float, r_f: float) -> float:
        """
        PV ≈ discounted notional * (spot - strike * DFf/DFd)
        For simplicity, assume DFd = exp(-r_d*T), DFf = exp(-r_f*T).
        """
        T = self.maturity
        df_d = np.exp(-r_d*T)
        df_f = np.exp(-r_f*T)
        fwd_theoretical = fx * df_f / df_d
        pv = self.notional_base * (fwd_theoretical - self.strike) * df_d
        return pv if self.direction == "buy_base" else -pv

@dataclass
class EuropeanOption:
    notional: float
    maturity: float
    strike: float
    option_type: Literal["call","put"]
    underlying: Literal["equity","fx"] = "equity"

    def pv_black_scholes(self, s: float, r: float, sigma: float, T: float) -> float:
        """
        Black–Scholes PV using an error-function approximation for N(x).
        """
        if T <= 0:
            if self.option_type == "call":
                return max(s - self.strike, 0.0) * self.notional
            else:
                return max(self.strike - s, 0.0) * self.notional

        def N(x):
            import math
            return 0.5 * (1.0 + math.erf(x / np.sqrt(2.0)))

        d1 = (np.log(s/self.strike) + (r + 0.5*sigma*sigma)*T)/(sigma*np.sqrt(T))
        d2 = d1 - sigma*np.sqrt(T)
        if self.option_type == "call":
            return self.notional * (s*N(d1) - np.exp(-r*T)*self.strike*N(d2))
        else:
            return self.notional * (np.exp(-r*T)*self.strike*N(-d2) - s*N(-d1))
