#include "henry_isotherm.hpp"

#include <cmath>
#include <sstream>

HenryIsotherm::HenryIsotherm(double k) :
    mK {k}
    {

    }

double
HenryIsotherm::loading(double P) const
    {
    return  mK * P;
    }

double
HenryIsotherm::spressure(double P) const
    {
    return mK * P;
    }

std::string
HenryIsotherm::getInfoString() const
    {
    std::stringstream ss;
    ss << "henry" << '\n';
    for (const auto& e : getParameters())
        ss << e.first << "  " << e.second << '\n';

    return ss.str();
    }

Isotherm::ParameterType
HenryIsotherm::getParameters() const
    {
    ParameterType params;
    params["k"] = mK;

    return params;
    }
