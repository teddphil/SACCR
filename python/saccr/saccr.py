from dataclasses import dataclass
from typing import List
import numpy as np

SUPERVISORY_FACTORS = {
    "IR": 0.005,   # 0.5% (toy)
    "FX": 0.04,    # 4%   (toy)
    "EQ": 0.08,    # 8%   (toy)
}

ALPHA = 1.4  # SA-CCR alpha (constant)

@dataclass
class TradeAddon:
    asset_class: str         # "IR","FX","EQ"
    notional_equivalent: float
    maturity: float          # years

def addon_ir(notional: float, maturity: float) -> float:
    sf = SUPERVISORY_FACTORS["IR"]
    effective_maturity = min(max(maturity, 0.25), 5.0)
    return sf * notional * np.sqrt(effective_maturity)

def addon_fx(notional: float, maturity: float) -> float:
    sf = SUPERVISORY_FACTORS["FX"]
    return sf * notional * np.sqrt(max(maturity, 0.25))

def addon_eq(notional: float, maturity: float) -> float:
    sf = SUPERVISORY_FACTORS["EQ"]
    return sf * notional * np.sqrt(max(maturity, 0.25))

def pfe_addon(addons: List[TradeAddon]) -> float:
    total = 0.0
    for a in addons:
        if a.asset_class == "IR":
            total += addon_ir(a.notional_equivalent, a.maturity)
        elif a.asset_class == "FX":
            total += addon_fx(a.notional_equivalent, a.maturity)
        elif a.asset_class == "EQ":
            total += addon_eq(a.notional_equivalent, a.maturity)
        else:
            raise ValueError("Unknown asset class")
    return total

def replacement_cost(current_mtm: float, collateral: float = 0.0) -> float:
    return max(current_mtm - collateral, 0.0)

def saccr_ead_educational(current_mtm: float, addons: List[TradeAddon], collateral: float = 0.0):
    rc = replacement_cost(current_mtm, collateral)
    pfe = pfe_addon(addons)
    ead = ALPHA * (rc + pfe)
    return {"RC": rc, "PFE_addon": pfe, "alpha": ALPHA, "EAD": ead}
