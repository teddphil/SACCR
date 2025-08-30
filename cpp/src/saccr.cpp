#include "saccr.hpp"
#include <cmath>
#include <stdexcept>

static const double SF_IR = 0.005;
static const double SF_FX = 0.04;
static const double SF_EQ = 0.08;
static const double ALPHA = 1.4;

static double addon_ir(double notional, double maturity) {
    double eff = std::min(std::max(maturity, 0.25), 5.0);
    return SF_IR * notional * std::sqrt(eff);
}

static double addon_fx(double notional, double maturity) {
    return SF_FX * notional * std::sqrt(std::max(maturity, 0.25));
}

static double addon_eq(double notional, double maturity) {
    return SF_EQ * notional * std::sqrt(std::max(maturity, 0.25));
}

double replacement_cost(double current_mtm, double collateral) {
    return std::max(current_mtm - collateral, 0.0);
}

SaccrResult saccr_ead_educational(double current_mtm, const std::vector<TradeAddon>& addons, double collateral) {
    double rc = replacement_cost(current_mtm, collateral);
    double pfe = 0.0;
    for (const auto &a: addons) {
        if (a.asset_class == "IR") pfe += addon_ir(a.notional_equivalent, a.maturity);
        else if (a.asset_class == "FX") pfe += addon_fx(a.notional_equivalent, a.maturity);
        else if (a.asset_class == "EQ") pfe += addon_eq(a.notional_equivalent, a.maturity);
        else throw std::runtime_error("Unknown asset class");
    }
    double ead = ALPHA * (rc + pfe);
    return SaccrResult{rc, pfe, ALPHA, ead};
}
