#pragma once
#include "market.hpp"
#include "trades.hpp"
#include <vector>

struct MonteCarloEngine {
    MarketParams mkt;
    unsigned long n_paths = 10000;
    unsigned int seed = 42;

    MonteCarloEngine(const MarketParams& m, unsigned long n=10000, unsigned int s=42) : mkt(m), n_paths(n), seed(s) {}

    // simulate simple Gaussian rates and GBM for underlying; return exposures as 2D vectors (paths x steps)
    std::vector<double> time_grid(double T, int steps);
    std::vector<std::vector<double>> simulate_rates(double T, int steps);
    std::vector<std::vector<double>> simulate_gbm(double s0, double mu, double sigma, double T, int steps);
    std::vector<std::vector<double>> exposure_profile_swap(const IRSwap& swap, int steps=24);
    std::vector<std::vector<double>> exposure_profile_fx_fwd(const FXForward& fxfwd, int steps=24, double r_foreign=0.02);
    std::vector<std::vector<double>> exposure_profile_option(const EuropeanOption& opt, int steps=24, double r_foreign=0.02);

    static std::vector<double> epe(const std::vector<std::vector<double>>& exposure_paths);
    static std::vector<double> pfe(const std::vector<std::vector<double>>& exposure_paths, double quantile=0.95);
};
