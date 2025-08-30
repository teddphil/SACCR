from __future__ import annotations
from dataclasses import dataclass, field
from typing import List, Union
import numpy as np

from .trades import IRSwap, FXForward, EuropeanOption
Trade = Union[IRSwap, FXForward, EuropeanOption]

@dataclass
class NettingSet:
    trades: List[Trade] = field(default_factory=list)

    def add(self, trade: Trade):
        self.trades.append(trade)

    def aggregate_exposure(self, pathwise_pvs: list):
        total = sum(pathwise_pvs)
        exposure = np.maximum(total, 0.0)
        return exposure
