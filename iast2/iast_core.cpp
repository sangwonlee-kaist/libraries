#include "iast_core.hpp"

#include <sstream>

#include "solver.hpp"
#include "solver_factory.hpp"

std::shared_ptr<Isotherm>&
Iast::operator [] (int i)
    {
    if (mIsotherms.size() <= static_cast<std::size_t>(i))
        throw IastException {__FILE__, __LINE__, "index > The number of isotherms."};

    return mIsotherms[i];
    }

Iast&
Iast::setIsotherms(IsothermVector isothermVector)
    {
    mIsotherms = isothermVector;
    return *this;
    }

Iast&
Iast::calculate(int mode, ValueType pOrN, std::vector<ValueType> composition)
    {
    if (mIsotherms.size() < 2)
        throw IastException {__FILE__, __LINE__, "# of isotherms < 2."};

    if (composition.size() < 2)
        throw IastException {__FILE__, __LINE__, "# of components < 2."};

    if (mIsotherms.size() != composition.size())
        throw IastException {__FILE__, __LINE__, "# of isotherms != # of components."};

    switch (mode)
        {
        case Mode::FIX_PY:
            this->modeFixPy(pOrN, composition);
            break;
        case Mode::FIX_PX:
            this->modeFixPx(pOrN, composition);
            break;
        case Mode::FIX_NX:
            this->modeFixNx(pOrN, composition);
            break;
        default:
            throw IastException {__FILE__, __LINE__, "Unsupported IAST mode."};
        }

    return *this;
    }

void
Iast::modeFixPy(ValueType p, std::vector<ValueType> y)
    {
    SolverFactory factory;
    std::shared_ptr<Solver> solver = factory.create("simplex");

    std::vector<Solver::FunctionType> functions;

    int dim = y.size();

    // Make objective functions
    int pivot = 0;
    for (int i = 0; i < dim; ++i)
        {
        if (i == pivot)
            continue;

        functions.push_back([this, i, p, y, pivot](const Solver::PointType& x)
            {
            int j = pivot;
            return mIsotherms[i]->spressure(p * y[i] / x[i]) /
                   mIsotherms[j]->spressure(p * y[j] / x[j]) - 1.0;
            });
        }

    functions.push_back([](const Solver::PointType& x)
        {
        double sum = 0.0;

        for (const auto& e : x)
            sum += e;

        return sum - 1.0;
        });

    // Make initial guess.
    std::vector<ValueType> x;
    x.resize(dim);

    for (int i = 0; i < dim; ++i)
        x[i] = mIsotherms[i]->loading(p * y[i]);

    try {
        // Solve!
        solver->setFunctions(functions).
                setInitialPoint(x);

        solver->solve();
        }
    catch (SolverException& e)
        {
        std::stringstream ss;
        ss << "Iast calculation fails.\n";
        ss << "    Reason: " << e.what();

        throw IastException {__FILE__, __LINE__, ss.str()};
        }

    x = solver->getRootPoint();

    // Make output.
    ValueType totalUptake = 0.0;
    for (int i = 0; i < dim; ++i)
        totalUptake += x[i] / mIsotherms[i]->loading(p * y[i] / x[i]);
    totalUptake = 1.0 / totalUptake;

    mResult = {totalUptake, x};
    }

void
Iast::modeFixPx(ValueType p, std::vector<ValueType> x)
    {
    // Create solver.
    SolverFactory factory;
    std::shared_ptr<Solver> solver = factory.create("simplex");

    std::vector<Solver::FunctionType> functions;

    int dim = x.size();

    // Make objective functions
    int pivot = 0;
    for (int i = 0; i < dim; ++i)
        {
        if (i == pivot)
            continue;

        functions.push_back([this, i, p, x, pivot](const Solver::PointType& y)
            {
            int j = pivot;
            return mIsotherms[i]->spressure(p * y[i] / x[i]) /
                   mIsotherms[j]->spressure(p * y[j] / x[j]) - 1.0;
            });
        }

    functions.push_back([](const Solver::PointType& y)
        {
        double sum = 0.0;

        for (const auto& e : y)
            sum += e;

        return sum - 1.0;
        });

    // Make initial guess.
    std::vector<ValueType> y;
    y.resize(dim);

    for (int i = 0; i < dim; ++i)
        y[i] = x[i];

    try {
        // Solve!
        solver->setFunctions(functions).
                setInitialPoint(x);

        solver->solve();
        }
    catch (SolverException& e)
        {
        std::stringstream ss;
        ss << "Iast calculation fails.\n";
        ss << "    Reason: " << e.what();

        throw IastException {__FILE__, __LINE__, ss.str()};
        }

    y = solver->getRootPoint();

    // Make output.
    ValueType totalUptake = 0.0;
    for (int i = 0; i < dim; ++i)
        totalUptake += x[i] / mIsotherms[i]->loading(p * y[i] / x[i]);
    totalUptake = 1.0 / totalUptake;

    mResult = {totalUptake, y};
    }

void
Iast::modeFixNx(ValueType n, std::vector<ValueType> x)
    {
    n = 0.0;
    x = {n};
    x = x;
    throw IastException {__FILE__, __LINE__, "Unsupported mode now."};
    mResult = {3.0, {3.0}};
    }

Iast::ResultType
Iast::getResult() const
    {
    return mResult;
    }