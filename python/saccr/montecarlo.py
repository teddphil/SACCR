import numpy as np
from .market import time_grid_years, MarketParams

class MonteCarloEngine:
    def __init__(self, market: MarketParams, n_paths: int = 10000, seed: int = 42):
        self.mkt = market
        self.n_paths = n_paths
        self.rng = np.random.default_rng(seed)

    def simulate_rates(self, T: float, steps: int):
        t, dt = time_grid_years(T, steps)
        r = np.empty((self.n_paths, steps+1))
        r[:,0] = self.mkt.r0
        for i in range(1, steps+1):
            z = self.rng.standard_normal(self.n_paths)
            r[:,i] = r[:,i-1] + self.mkt.sigma_r * np.sqrt(dt[i]) * z
        return t, r

    def simulate_gbm(self, s0: float, mu: float, sigma: float, T: float, steps: int):
        t, dt = time_grid_years(T, steps)
        s = np.empty((self.n_paths, steps+1))
        s[:,0] = s0
        for i in range(1, steps+1):
            z = self.rng.standard_normal(self.n_paths)
            s[:,i] = s[:,i-1] * np.exp((mu - 0.5*sigma*sigma)*dt[i] + sigma*np.sqrt(dt[i])*z)
        return t, s

    def exposure_profile_swap(self, swap, steps: int = 24):
        T = swap.maturity
        t, r = self.simulate_rates(T, steps)
        pv = np.array([swap.pv(r[:,i]) for i in range(steps+1)]).T
        exp = np.maximum(pv, 0.0)
        return t, exp

    def exposure_profile_fx_fwd(self, fxfwd, steps: int = 24, r_foreign: float = 0.02):
        T = fxfwd.maturity
        t, fx = self.simulate_gbm(self.mkt.fx0, self.mkt.mu_fx, self.mkt.sigma_fx, T, steps)
        r_d = self.mkt.discount_rate
        r_f = r_foreign
        pv = np.empty_like(fx)
        for i in range(fx.shape[1]):
            pv[:,i] = fxfwd.pv(fx[:,i], r_d, r_f)
        exp = np.maximum(pv, 0.0)
        return t, exp

    def exposure_profile_euro_option(self, opt, steps: int = 24, on_fx=False, r_foreign: float = 0.02):
        T = opt.maturity
        if on_fx:
            t, s = self.simulate_gbm(self.mkt.fx0, self.mkt.mu_fx, self.mkt.sigma_fx, T, steps)
        else:
            t, s = self.simulate_gbm(self.mkt.s0, self.mkt.mu_eq, self.mkt.sigma_eq, T, steps)
        r = self.mkt.discount_rate
        pv = np.empty_like(s)
        for i in range(s.shape[1]):
            tau = T * (1.0 - i/steps)
            pv[:,i] = np.maximum(0.0, opt.pv_black_scholes(s[:,i], r, self.mkt.sigma_eq if not on_fx else self.mkt.sigma_fx, max(tau, 1e-8)))
        exp = np.maximum(pv, 0.0)
        return t, exp

    @staticmethod
    def epe_pfe(exposure_paths, quantile=0.95):
        epe = exposure_paths.mean(axis=0)
        pfe = np.quantile(exposure_paths, quantile, axis=0)
        return epe, pfe
