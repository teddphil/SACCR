import argparse
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

from .market import MarketParams
from .trades import IRSwap, FXForward, EuropeanOption
from .montecarlo import MonteCarloEngine

def run_example(n_paths=5000, steps=24, plot=False):
    mkt = MarketParams()
    mc = MonteCarloEngine(mkt, n_paths=n_paths, seed=123)

    swap = IRSwap(notional=1e6, maturity=2.0, fixed_rate=0.03, pay_fixed=True)
    fxfwd = FXForward(notional_base=1e6, maturity=1.0, strike=1.25, direction="buy_base")
    opt = EuropeanOption(notional=1e6, maturity=0.75, strike=100.0, option_type="call", underlying="equity")

    t1, exp_swap = mc.exposure_profile_swap(swap, steps=steps)
    t2, exp_fxf = mc.exposure_profile_fx_fwd(fxfwd, steps=steps)
    t3, exp_opt  = mc.exposure_profile_euro_option(opt, steps=steps, on_fx=False)

    epe_swap, pfe_swap = mc.epe_pfe(exp_swap)
    epe_fxf , pfe_fxf  = mc.epe_pfe(exp_fxf)
    epe_opt , pfe_opt  = mc.epe_pfe(exp_opt)

    df = pd.DataFrame({
        "t_years": t1,
        "EPE_swap": epe_swap,
        "PFE95_swap": pfe_swap,
        "EPE_fxfwd": epe_fxf,
        "PFE95_fxfwd": pfe_fxf,
        "EPE_option": epe_opt,
        "PFE95_option": pfe_opt,
    })

    if plot:
        plt.figure()
        plt.plot(t1, epe_swap, label="EPE swap")
        plt.plot(t1, pfe_swap, label="PFE95 swap")
        plt.plot(t1, epe_fxf , label="EPE FX fwd")
        plt.plot(t1, pfe_fxf , label="PFE95 FX fwd")
        plt.plot(t1, epe_opt , label="EPE option")
        plt.plot(t1, pfe_opt , label="PFE95 option")
        plt.xlabel("Time (years)")
        plt.ylabel("Exposure")
        plt.legend()
        plt.title("Exposure Profiles (Educational)")
        plt.show()

    return df

def main():
    parser = argparse.ArgumentParser(description="Educational SA-CCR Exposure Calculator")
    parser.add_argument("--paths", type=int, default=5000, help="Monte Carlo paths")
    parser.add_argument("--steps", type=int, default=24, help="Time steps")
    parser.add_argument("--plot", action="store_true", help="Plot exposure profiles")
    parser.add_argument("--example", action="store_true", help="Run example portfolio")
    args = parser.parse_args()

    if args.example:
        df = run_example(n_paths=args.paths, steps=args.steps, plot=args.plot)
        print(df.head().to_string(index=False))
    else:
        print("Use --example to run the demo.")

if __name__ == "__main__":
    main()
