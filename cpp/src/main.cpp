#include <iostream>
#include <vector>
#include <iomanip>
#include "market.hpp"
#include "montecarlo.hpp"
#include "trades.hpp"
#include "saccr.hpp"

int main() {
    MarketParams mkt;
    MonteCarloEngine mc(mkt, 5000, 123);

    IRSwap swap{1e6, 2.0, 0.03, true, NAN};
    FXForward fxfwd{1e6, 1.0, 1.25, true};
    EuropeanOption opt{1e6, 0.75, 100.0, true, false};

    auto exp_swap = mc.exposure_profile_swap(swap, 24);
    auto exp_fx = mc.exposure_profile_fx_fwd(fxfwd, 24);
    auto exp_opt = mc.exposure_profile_option(opt, 24);

    auto epe_swap = MonteCarloEngine::epe(exp_swap);
    auto pfe_swap = MonteCarloEngine::pfe(exp_swap, 0.95);

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "t,EPE_swap,PFE95_swap,EPE_fx,PFE95_fx,EPE_opt,PFE95_opt\n";
    auto tgrid = mc.time_grid(2.0, 24);
    for (size_t i=0;i<tgrid.size();++i) {
        double t = tgrid[i];
        double epe_s = (i < epe_swap.size()) ? epe_swap[i] : 0.0;
        double pfe_s = (i < pfe_swap.size()) ? pfe_swap[i] : 0.0;
        double epe_fx = (i < MonteCarloEngine::epe(exp_fx).size()) ? MonteCarloEngine::epe(exp_fx)[i] : 0.0;
        double pfe_fx = (i < MonteCarloEngine::pfe(exp_fx).size()) ? MonteCarloEngine::pfe(exp_fx)[i] : 0.0;
        double epe_o = (i < MonteCarloEngine::epe(exp_opt).size()) ? MonteCarloEngine::epe(exp_opt)[i] : 0.0;
        double pfe_o = (i < MonteCarloEngine::pfe(exp_opt).size()) ? MonteCarloEngine::pfe(exp_opt)[i] : 0.0;
        std::cout << t << "," << epe_s << "," << pfe_s << "," << epe_fx << "," << pfe_fx << "," << epe_o << "," << pfe_o << "\n";
    }

    // Example SA-CCR EAD calculation
    std::vector<TradeAddon> addons = {
        {"IR", 1e6, 2.0},
        {"FX", 1e6, 1.0},
        {"EQ", 1e6, 0.75}
    };
    double current_mtm = 10000.0; // example MTM
    auto res = saccr_ead_educational(current_mtm, addons, 0.0);
    std::cout << "\nSA-CCR Educational Results:\n";
    std::cout << "RC: " << res.RC << ", PFE_addon: " << res.PFE_addon << ", alpha: " << res.alpha << ", EAD: " << res.EAD << "\n";
    return 0;
}
