#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

#include "optimization.h"

int main(int argc, char *argv[])
{
    if (argc != 5)
    {
        std::cerr << "Usage: richqp (budget) (betas file) (incomes file) (output aid file)" << std::endl;
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

    std::ofstream aidsfile(argv[4]);
    if (!aidsfile)
    {
        std::cerr << "Couldn't open output aids file " << argv[4] << std::endl;
        return -1;
    }

    

    bool posdef = true;
    
    alglib::minqpstate state;
    alglib::minqpcreate(nvars, state);

    alglib::sparsematrix a;
    alglib::sparsecreate(nvars, nvars, 0, a);
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

    alglib::real_2d_array c;
    c.setlength(1, nvars + 1);
    for (int i = 0; i < nvars; i++)
    {
        c[0][i] = 1.0;        
    }
    c[0][nvars] = nvars * budget;
    alglib::integer_1d_array ct = "[0]";
    alglib::minqpsetlc(state, c, ct);

    alglib::real_1d_array scales;
    scales.setlength(nvars);
    for (int i = 0; i < nvars; i++)
        scales[i] = budget;

    alglib::minqpsetscale(state, scales);
    alglib::minqpsetalgobleic(state, 0, 0, 0, 0);

    alglib::minqpoptimize(state);

    alglib::minqpreport rep;
    alglib::real_1d_array aids;
    alglib::minqpresults(state, aids, rep);

    std::cout << "Terminated in " << rep.outeriterationscount << " iterations, termination code: " << rep.terminationtype << std::endl;

    for (int i = 0; i < nvars; i++)
    {
        aidsfile << aids[i] << std::endl;
    }

    return 0;
}