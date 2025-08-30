#pragma once
#include <cmath>

struct IRSwap {
    double notional;
    double maturity; // years
    double fixed_rate;
    bool pay_fixed;
    double dv01_bp = NAN;

    double pv(double r) {
        if (std::isnan(dv01_bp)) {
            double df = std::exp(-r * maturity);
            dv01_bp = notional * maturity * df / 10000.0;
        }
        double sign = pay_fixed ? 1.0 : -1.0;
        return sign * dv01_bp * 10000.0 * (r - fixed_rate);
    }
};

struct FXForward {
    double notional_base;
    double maturity;
    double strike;
    bool buy_base; // true: buy base, false: sell base

    double pv(double fx, double r_d, double r_f) const {
        double T = maturity;
        double df_d = std::exp(-r_d * T);
        double df_f = std::exp(-r_f * T);
        double fwd_theoretical = fx * df_f / df_d;
        double pv = notional_base * (fwd_theoretical - strike) * df_d;
        return buy_base ? pv : -pv;
    }
};

struct EuropeanOption {
    double notional;
    double maturity;
    double strike;
    bool is_call;
    bool on_fx; // if true, underlying is FX, else equity

    // cumulative normal via erf-based approximation
    static double N(double x) {
        return 0.5 * (1.0 + std::erf(x / std::sqrt(2.0)));
    }

    static double bs_price(double s, double K, double r, double sigma, double T, bool call) {
        if (T <= 0.0) {
            if (call) return std::max(s - K, 0.0);
            else return std::max(K - s, 0.0);
        }
        double sqrtT = std::sqrt(T);
        double d1 = (std::log(s / K) + (r + 0.5 * sigma * sigma) * T) / (sigma * sqrtT);
        double d2 = d1 - sigma * sqrtT;
        if (call) return s * N(d1) - std::exp(-r * T) * K * N(d2);
        else return std::exp(-r * T) * K * N(-d2) - s * N(-d1);
    }

    double pv(double s, double r, double sigma, double tau) const {
        double price = bs_price(s, strike, r, sigma, tau, is_call);
        return notional * price;
    }
};
