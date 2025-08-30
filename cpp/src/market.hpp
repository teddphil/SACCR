#pragma once
struct MarketParams {
    double r0 = 0.03;
    double sigma_r = 0.01;
    double mu_fx = 0.0;
    double sigma_fx = 0.10;
    double mu_eq = 0.05;
    double sigma_eq = 0.20;
    double fx0 = 1.25;
    double s0 = 100.0;
    double discount_rate = 0.03;
};
