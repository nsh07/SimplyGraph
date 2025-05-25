//
// Created by nishant on 24/05/25.
//

#ifndef SIMPLYGRAPH_FUNCS_HPP
#define SIMPLYGRAPH_FUNCS_HPP

#include <string>
#include <numbers>

inline bool approxEqual(double a, double b, double eps) {
    return (a != 0.0 ? abs((a - b) / a) <= eps : a == b);
}

inline std::string &rtrim(std::string &s, const char *t = " \t\n\r\f\v") {
    s.erase(s.find_last_not_of(t) + 1);
    return s;
}

inline std::string &ltrim(std::string &s, const char *t = " \t\n\r\f\v") {
    s.erase(0, s.find_first_not_of(t));
    return s;
}

inline std::string &trim(std::string &s, const char *t = " \t\n\r\f\v") {
    return ltrim(rtrim(s, t), t);
}

inline void addConstants(exprtk::symbol_table<double> &symbolTable) {
    symbolTable.add_constant("e", std::numbers::e);
    symbolTable.add_constant("π", std::numbers::pi); // "pi" is already added by add_constants()
    symbolTable.add_constant("phi", std::numbers::phi);
    symbolTable.add_constant("ϕ", std::numbers::phi);
    symbolTable.add_constant("φ", std::numbers::phi);
    symbolTable.add_constant("gamma", std::numbers::egamma);
    symbolTable.add_constant("γ", std::numbers::egamma);
}

#endif //SIMPLYGRAPH_FUNCS_HPP
