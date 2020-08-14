#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

#include "optimization.h"

void runOptimization(alglib::minqpstate &state, int nvars, double totalBudget)
{
    alglib::real_2d_array c;
    c.setlength(1, nvars + 1);
    for (int i = 0; i < nvars; i++)
    {
        c[0][i] = 1.0;        
    }
    c[0][nvars] = totalBudget;
    alglib::integer_1d_array ct = "[-1]";
    alglib::minqpsetlc(state, c, ct);

    alglib::minqpoptimize(state);
}

int main(int argc, char *argv[])
{    
    if (argc != 8)
    {
        std::cerr << "Usage: richqp (budget) (betas file) (incomes file) (actual aids file) (output optimal government aid file) (output optimal university aid file) (output equal-outcome aids file)" << std::endl;
        return -1;
    }        
    double budget = std::strtod(argv[1], NULL);
    std::ifstream betasfile(argv[2]);
    if (!betasfile)
    {
        std::cerr << "Couldn't open betas file: " << argv[2] << std::endl;
        return -1;
    }

    std::vector<double> betas[3];
    
    for (int i = 0; i < 3 && betasfile; i++)
    {
        std::string betaline;
        std::getline(betasfile, betaline);
        std::stringstream ss(betaline);

        int betacnt = 0;

        while (true)
        {
            double val;
            ss >> val;
            if (!ss)
                break;
            betas[i].push_back(val);
            betacnt++;
        }

        std::cout << "Read " << betacnt << " betas from line " << i << std::endl;
    }

    if (betas[2].size() == 0)
    {
        std::cerr << "Need at least one quadratic term in the objective function!" << std::endl;
        return -1;
    }

    std::vector<double> incomes;
    std::ifstream incomesfile(argv[3]);
    if (!incomesfile)
    {
        std::cerr << "Couldn't open incomes file: " << argv[3] << std::endl;
        return -1;
    }

    while (true)
    {
        double val;
        incomesfile >> val;
        if (!incomesfile)
            break;
        incomes.push_back(val);
    }

    int nvars = incomes.size();
    std::cout << "Read " << nvars << " incomes" << std::endl;


    std::vector<double> actualaids;
    std::ifstream actualaidsfile(argv[4]);
    if (!actualaidsfile)
    {
        std::cerr << "Couldn't open actual aids file: " << argv[4] << std::endl;
        return -1;
    }

    while (true)
    {
        double val;
        actualaidsfile >> val;
        if (!actualaidsfile)
            break;
        actualaids.push_back(val);
    }
    if (actualaids.size() != nvars)
    {
        std::cerr << "Number of aids values (" << actualaids.size() << ") does not match number of incomes (" << nvars << ")!" << std::endl;
        return -1;
    }


    std::ofstream govaidsfile(argv[5]);
    if (!govaidsfile)
    {
        std::cerr << "Couldn't open output optimal government aids file " << argv[5] << std::endl;
        return -1;
    }

    std::ofstream uniaidsfile(argv[6]);
    if (!uniaidsfile)
    {
        std::cerr << "Couldn't open output optimal university aids file " << argv[6] << std::endl;
        return -1;
    }

    std::ofstream equalaidsfile(argv[7]);
    if (!equalaidsfile)
    {
        std::cerr << "Couldn't open output equal-outcome aids file " << argv[7] << std::endl;
        return -1;
    }

    bool posdef = true;
    
    alglib::minqpstate state;
    alglib::minqpcreate(nvars, state);

    alglib::sparsematrix a;
    alglib::sparsecreate(nvars, nvars, 0, a);
    std::vector<double> quadcoeffs(nvars);
    for (int i = 0; i < nvars; i++)
    {
        double incterm = 1.0;
        double coeff = 0.0;
        for (auto beta : betas[2])
        {
            coeff += -2.0 * incterm * beta;
            incterm *= incomes[i];
        }
        if (coeff < 0)
            posdef = false;
        alglib::sparseset(a, i, i, coeff);
        quadcoeffs[i] = 0.5 * coeff;
    }
    alglib::minqpsetquadratictermsparse(state, a, true);

    if (!posdef)
    {
        std::cerr << "Warning: problem is not convex!" << std::endl;        
    }

    alglib::real_1d_array b;
    b.setlength(nvars);
    for (int i = 0; i < nvars; i++)
    {
        double incterm = 1.0;
        double coeff = 0.0;
        for (auto beta : betas[1])
        {
            coeff += -1.0 * incterm * beta;
            incterm *= incomes[i];
        }
        b[i] = coeff;
    }
    alglib::minqpsetlinearterm(state, b);

    double constterm = 0;
    for (int i = 0; i < nvars; i++)
    {
        double incterm = 1.0;
        for (auto beta : betas[1])
        {
            constterm += -1.0 * incterm * beta;
            incterm *= incomes[i];
        }        
    }
    
    alglib::real_1d_array lb;
    alglib::real_1d_array ub;
    lb.setlength(nvars);
    ub.setlength(nvars);
    for (int i = 0; i < nvars; i++)
    {
        lb[i] = 0;
        ub[i] = alglib::fp_posinf;
    }
    alglib::minqpsetbc(state, lb, ub);

    alglib::real_1d_array scales;
    scales.setlength(nvars);
    for (int i = 0; i < nvars; i++)
        scales[i] = budget;

    alglib::minqpsetscale(state, scales);
    alglib::minqpsetalgobleic(state, 0, 0, 0, 0);    

    runOptimization(state, nvars, nvars * budget);

    alglib::minqpreport rep;
    alglib::real_1d_array aids;
    alglib::minqpresults(state, aids, rep);
    
    double totaloptaid = 0;
    double totalbenefit = constterm;
    for (int i = 0; i < nvars; i++)
    {
        totaloptaid += aids[i];
        totalbenefit += quadcoeffs[i] * aids[i] * aids[i] + b[i] * aids[i];
    }

    if (std::fabs(totaloptaid - nvars * budget) > 1e-4)
    {
        std::cerr << "Warning: optimal aid allocation does not use the full allowed budget" << std::endl;
    }

    std::cout << "Optimal aid allocation (with government maximum budget " << nvars*budget << ") yields benefit " << -totalbenefit << std::endl;
    
    for (int i = 0; i < nvars; i++)
    {
        govaidsfile << aids[i] << std::endl;
    }


    double actualtotalaid = 0;
    double actualbenefit = constterm;
    for (int i = 0; i < nvars; i++)
    {
        actualtotalaid += actualaids[i];
        actualbenefit += quadcoeffs[i] * actualaids[i] * actualaids[i] + b[i] * actualaids[i];
    }
    std::cout << "Actual aid allocation uses total budget " << actualtotalaid << " and yields benefit " << -actualbenefit << std::endl;


    runOptimization(state, nvars, actualtotalaid);
    alglib::minqpresults(state, aids, rep);
    double unibenefit = constterm;
    for (int i = 0; i < nvars; i++)
    {
        unibenefit += quadcoeffs[i] * aids[i] * aids[i] + b[i] * aids[i];
    }
    std::cout << "Optimal aid allocation (with university maximum budget " << actualtotalaid << ") yields benefit " << -unibenefit << std::endl;
    for (int i = 0; i < nvars; i++)
    {
        uniaidsfile << aids[i] << std::endl;
    }

    double low = 0;
    double high = actualtotalaid;
    while (high - low > 1e-6)
    {
        double mid = 0.5 * (low + high);
        runOptimization(state, nvars, mid);
        alglib::minqpresults(state, aids, rep);
        double newbenefit = constterm;
        for (int i = 0; i < nvars; i++)
        {            
            newbenefit += quadcoeffs[i] * aids[i] * aids[i] + b[i] * aids[i];
        }
        if (newbenefit <= actualbenefit)
            high = mid;
        else
            low = mid;
    }
    std::cout << "Optimal total budget needed to get benefit " << -actualbenefit << " is " << high << std::endl;

    for (int i = 0; i < nvars; i++)
    {
        equalaidsfile << aids[i] << std::endl;
    }

    return 0;
}