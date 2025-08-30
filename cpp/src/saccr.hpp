#pragma once
#include <vector>
#include <string>

struct TradeAddon {
    std::string asset_class; // "IR","FX","EQ"
    double notional_equivalent;
    double maturity;
};

struct SaccrResult {
    double RC;
    double PFE_addon;
    double alpha;
    double EAD;
};

SaccrResult saccr_ead_educational(double current_mtm, const std::vector<TradeAddon>& addons, double collateral=0.0);
