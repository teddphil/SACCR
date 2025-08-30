import numpy as np
from saccr.market import MarketParams
from saccr.trades import IRSwap, FXForward, EuropeanOption
from saccr.montecarlo import MonteCarloEngine

def test_basic_runs():
    mkt = MarketParams()
    mc = MonteCarloEngine(mkt, n_paths=1000, seed=7)

    swap = IRSwap(notional=1e6, maturity=1.0, fixed_rate=0.03, pay_fixed=True)
    t, exp = mc.exposure_profile_swap(swap, steps=12)
    assert exp.shape[0] == 1000
    assert exp.shape[1] == 13
    assert np.all(exp >= 0.0)

    fxf = FXForward(notional_base=1e6, maturity=1.0, strike=1.25, direction="buy_base")
    t, exp = mc.exposure_profile_fx_fwd(fxf, steps=12)
    assert exp.shape[0] == 1000
    assert np.all(exp >= 0.0)

    opt = EuropeanOption(notional=1e6, maturity=0.5, strike=100.0, option_type="call", underlying="equity")
    t, exp = mc.exposure_profile_euro_option(opt, steps=12, on_fx=False)
    assert exp.shape[0] == 1000
    assert np.all(exp >= 0.0)
