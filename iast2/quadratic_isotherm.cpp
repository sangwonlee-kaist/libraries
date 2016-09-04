#include "quadratic_isotherm.hpp"

#include <sstream>
#include <cmath>

QuadraticIsotherm::QuadraticIsotherm(double _q, double _k1, double _k2) :
    q {_q}, k1 {_k1}, k2 {_k2}
    {
    }

QuadraticIsotherm::~QuadraticIsotherm()
    {
    }

double
QuadraticIsotherm::loading(double p) const
    {
    return q * (k1 + 2.0 * k2 * p) * p / (1.0 + k1 * p + k2 * p * p);
    }

double
QuadraticIsotherm::spressure(double p) const
    {
    return q * std::log(1.0 + k1 * p + k2 * p * p);
    }

std::string
QuadraticIsotherm::getInfoString() const
    {
    std::stringstream ss;
    ss << "[Quadratic Isotherm]\n" <<
          "[Parameters] q = " << q << ", k1 = " << k1 << ", k2 = " << k2;

    return ss.str();
    }
