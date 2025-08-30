#include "montecarlo.hpp"
#include <random>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <stdexcept>

std::vector<double> MonteCarloEngine::time_grid(double T, int steps) {
    std::vector<double> t(steps+1);
    for (int i=0;i<=steps;i++) t[i] = T * double(i)/double(steps);
    return t;
}

std::vector<std::vector<double>> MonteCarloEngine::simulate_rates(double T, int steps) {
    std::mt19937_64 rng(seed);
    std::normal_distribution<double> nd(0.0,1.0);
    std::vector<double> dt(steps+1);
    for (int i=0;i<=steps;i++) dt[i] = (i==0)?0.0:(T/double(steps));
    std::vector<std::vector<double>> r(n_paths, std::vector<double>(steps+1, mkt.r0));
    for (int i=1;i<=steps;i++) {
        double sqrt_dt = std::sqrt(dt[i]);
        for (unsigned long p=0;p<n_paths;p++) {
            double z = nd(rng);
            r[p][i] = r[p][i-1] + mkt.sigma_r * sqrt_dt * z;
        }
    }
    return r;
}

std::vector<std::vector<double>> MonteCarloEngine::simulate_gbm(double s0, double mu, double sigma, double T, int steps) {
    std::mt19937_64 rng(seed+1);
    std::normal_distribution<double> nd(0.0,1.0);
    std::vector<double> dt(steps+1);
    for (int i=0;i<=steps;i++) dt[i] = (i==0)?0.0:(T/double(steps));
    std::vector<std::vector<double>> s(n_paths, std::vector<double>(steps+1, s0));
    for (int i=1;i<=steps;i++) {
        double sqrt_dt = std::sqrt(dt[i]);
        for (unsigned long p=0;p<n_paths;p++) {
            double z = nd(rng);
            s[p][i] = s[p][i-1] * std::exp((mu - 0.5*sigma*sigma)*dt[i] + sigma*sqrt_dt*z);
        }
    }
    return s;
}

std::vector<std::vector<double>> MonteCarloEngine::exposure_profile_swap(const IRSwap& swap, int steps) {
    auto r = simulate_rates(swap.maturity, steps);
    std::vector<std::vector<double>> pv(n_paths, std::vector<double>(steps+1,0.0));
    for (int i=0;i<=steps;i++) {
        for (unsigned long p=0;p<n_paths;p++) {
            pv[p][i] = swap.pv(r[p][i]);
        }
    }
    std::vector<std::vector<double>> exp = pv;
    for (unsigned long p=0;p<n_paths;p++) for (int i=0;i<=steps;i++) exp[p][i] = std::max(pv[p][i], 0.0);
    return exp;
}

std::vector<std::vector<double>> MonteCarloEngine::exposure_profile_fx_fwd(const FXForward& fxfwd, int steps, double r_foreign) {
    auto s = simulate_gbm(mkt.fx0, mkt.mu_fx, mkt.sigma_fx, fxfwd.maturity, steps);
    std::vector<std::vector<double>> pv(n_paths, std::vector<double>(steps+1,0.0));
    for (int i=0;i<=steps;i++) {
        for (unsigned long p=0;p<n_paths;p++) {
            pv[p][i] = fxfwd.pv(s[p][i], mkt.discount_rate, r_foreign);
        }
    }
    std::vector<std::vector<double>> exp = pv;
    for (unsigned long p=0;p<n_paths;p++) for (int i=0;i<=steps;i++) exp[p][i] = std::max(pv[p][i], 0.0);
    return exp;
}

std::vector<std::vector<double>> MonteCarloEngine::exposure_profile_option(const EuropeanOption& opt, int steps, double r_foreign) {
    auto s = simulate_gbm(opt.on_fx ? mkt.fx0 : mkt.s0, opt.on_fx ? mkt.mu_fx : mkt.mu_eq, opt.on_fx ? mkt.sigma_fx : mkt.sigma_eq, opt.maturity, steps);
    std::vector<std::vector<double>> pv(n_paths, std::vector<double>(steps+1,0.0));
    for (int i=0;i<=steps;i++) {
        double tau = opt.maturity * (1.0 - double(i)/double(steps));
        double sigma = opt.on_fx ? mkt.sigma_fx : mkt.sigma_eq;
        for (unsigned long p=0;p<n_paths;p++) {
            pv[p][i] = opt.pv(s[p][i], mkt.discount_rate, sigma, std::max(tau, 1e-8));
        }
    }
    std::vector<std::vector<double>> exp = pv;
    for (unsigned long p=0;p<n_paths;p++) for (int i=0;i<=steps;i++) exp[p][i] = std::max(pv[p][i], 0.0);
    return exp;
}

std::vector<double> MonteCarloEngine::epe(const std::vector<std::vector<double>>& exposure_paths) {
    if (exposure_paths.empty()) return {};
    int steps = exposure_paths[0].size();
    std::vector<double> out(steps,0.0);
    for (int i=0;i<steps;i++) {
        double sum=0.0;
        for (auto &p: exposure_paths) sum += p[i];
        out[i] = sum / double(exposure_paths.size());
    }
    return out;
}

std::vector<double> MonteCarloEngine::pfe(const std::vector<std::vector<double>>& exposure_paths, double quantile) {
    if (exposure_paths.empty()) return {};
    int steps = exposure_paths[0].size();
    std::vector<double> out(steps,0.0);
    std::vector<double> tmp;
    tmp.reserve(exposure_paths.size());
    for (int i=0;i<steps;i++) {
        tmp.clear();
        for (auto &p: exposure_paths) tmp.push_back(p[i]);
        std::sort(tmp.begin(), tmp.end());
        size_t idx = std::min(tmp.size()-1, size_t(std::floor(quantile * tmp.size())));
        out[i] = tmp[idx];
    }
    return out;
}
