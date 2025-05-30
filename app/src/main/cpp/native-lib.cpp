//
// Created by nishant on 24/05/25.
//

#include <jni.h>

#include <cmath>
#include <regex>
#include <string>
#include <vector>

#include "exprtk.hpp"
#include "funcs.hpp"

typedef exprtk::symbol_table<double> symbol_table;
typedef exprtk::expression<double> expression;
typedef exprtk::parser<double> parser;

std::string jstring2string(JNIEnv *env, jstring jStr);

template<typename T>
void removeAlternatingPairs(std::vector<T> &vec);

extern "C"
JNIEXPORT jfloatArray JNICALL
Java_org_nsh07_simplygraph_NativeBridge_calculateGraphPoints(
        JNIEnv *env,
        jobject,
        jdouble xWidth,
        jdouble yWidth,
        jdouble xOffset,
        jdouble yOffset,
        jdouble canvasWidth,
        jdouble canvasHeight,
        jstring tStart,
        jstring tEnd,
        jstring thetaStart,
        jstring thetaEnd,
        jstring function
) {
    double xScaleFactor = canvasWidth / xWidth;
    double yScaleFactor = canvasHeight / yWidth;
    std::vector<float> points;

    std::string functionStr = jstring2string(env, function);

    auto equalsPos = functionStr.find('=');
    bool hasX = functionStr.find('x') != std::string::npos;
    bool hasY = functionStr.find('y') != std::string::npos;
    auto commaPos = functionStr.find(',');

    if (commaPos != std::string::npos) {
        // Parametric equation
        std::regex form(R"([\s]*\(.*,.*\)[\s]*)");
        if (std::regex_match(functionStr, form)) {
            // We evaluate and set the intervals for t first
            std::string tStartStr =
                    jstring2string(env, tStart), tEndStr = jstring2string(env, tEnd);

            symbol_table intervalSymbolTable;
            intervalSymbolTable.add_constants();
            addConstants(intervalSymbolTable);

            expression tStartExpression, tEndExpression;
            tStartExpression.register_symbol_table(intervalSymbolTable);
            tEndExpression.register_symbol_table(intervalSymbolTable);

            parser tStartParser, tEndParser;
            tStartParser.compile(tStartStr, tStartExpression);
            tEndParser.compile(tEndStr, tEndExpression);

            double t = tStartExpression.value();
            double tEndVal = tEndExpression.value();

            // Now we set up the function parsers
            std::string x_s = functionStr.substr(0, commaPos);
            x_s = x_s.substr(x_s.find('(') + 1);
            std::string y_s = functionStr.substr(commaPos + 1);
            y_s = y_s.substr(0, y_s.find_last_of(')'));

            symbol_table symbolTable;
            symbolTable.add_variable("t", t);
            symbolTable.add_constants();
            addConstants(symbolTable);

            expression xExpression, yExpression;
            xExpression.register_symbol_table(symbolTable);
            yExpression.register_symbol_table(symbolTable);

            parser xParser, yParser;
            xParser.compile(x_s, xExpression);
            yParser.compile(y_s, yExpression);

            double tIncr = abs((tEndVal - t) / (canvasWidth * 2));
            while (t <= tEndVal) {
                double x = xExpression.value() * xScaleFactor + canvasWidth / 2 + xOffset;
                double y = -yExpression.value() * yScaleFactor + canvasHeight / 2 + yOffset;

                if (!std::isnan(x) && !std::isnan(y)) {
                    points.push_back(float(x));
                    points.push_back(float(y));
                }
                t += tIncr;
            }
        }
    } else if (!hasX && !hasY) {
        std::string preEqual = functionStr.substr(0, equalsPos);
        bool isPolar = (equalsPos != std::string::npos) && (trim(preEqual) == "r");

        if (isPolar) {
            /*
            Polar equation
            This is very easy to draw, the polar equation is of the form r = f(theta), we iterate over
            various values of theta from 0 to 12pi, calculate r from the above equation, and add the point
            (r * cos(theta), r * sin(theta)) to the graph (since x = r cos(theta) and y = r sin(theta))
            */
            // We evaluate and set the intervals for theta first
            std::string thetaStartStr =
                    jstring2string(env, thetaStart), thetaEndStr = jstring2string(env, thetaEnd);

            symbol_table intervalSymbolTable;
            intervalSymbolTable.add_constants();
            addConstants(intervalSymbolTable);

            expression thetaStartExpression, thetaEndExpression;
            thetaStartExpression.register_symbol_table(intervalSymbolTable);
            thetaEndExpression.register_symbol_table(intervalSymbolTable);

            parser thetaStartParser, thetaEndParser;
            thetaStartParser.compile(thetaStartStr, thetaStartExpression);
            thetaEndParser.compile(thetaEndStr, thetaEndExpression);

            double theta = thetaStartExpression.value();
            double thetaEndVal = thetaEndExpression.value();

            // Now we set up the parser for calculating r
            double r;
            std::string rhs = functionStr.substr(equalsPos + 1);

            symbol_table symbolTable;
            symbolTable.add_variable("theta", theta);
            symbolTable.add_constants();
            addConstants(symbolTable);

            expression expression;
            expression.register_symbol_table(symbolTable);

            parser parser;
            parser.compile(rhs, expression);

            double thetaIncr = abs((thetaEndVal - theta) / (canvasWidth * 2));
            while (theta <= thetaEndVal) {
                r = expression.value();
                if (!std::isnan(r)) {
                    points.push_back(
                            float(r * cos(theta) * xScaleFactor + canvasWidth / 2 + xOffset));
                    points.push_back(
                            float(-r * sin(theta) * yScaleFactor + canvasHeight / 2 + yOffset));
                }
                theta += thetaIncr;
            }
        }
    } else if (!hasY) {
        // Explicit function of x (in the form f(x))

        double x;

        symbol_table symbolTable;
        symbolTable.add_variable("x", x);
        symbolTable.add_constants();
        addConstants(symbolTable);

        expression expression;
        expression.register_symbol_table(symbolTable);

        parser parser;
        parser.compile(functionStr, expression);

        for (int i = int(-xOffset); i <= canvasWidth - xOffset; i++) {
            try {
                // Calculate the value of x and y for the given i-pixel-offset
                // x can be simply calculated by subtracting half of canvas width (to shift origin to half of the screen) and then scaling according to xWidth
                x = ((i - canvasWidth / 2) / canvasWidth) * xWidth;

                // Calculating y is a bit more complex, we find the value of f(x) for x as calculated above,
                // then multiply it by the below factor to scale it to keep the x and y-axis scale uniform in the graph
                // We then subtract this value from half of the canvas width (to shift origin to half the screen)
                // And voila, we get a graph with the origin at the center of the screen.
                double y = (-expression.value() * yScaleFactor) + canvasHeight / 2;

                if (!std::isnan(y)) {
                    points.push_back(float(i + xOffset));
                    points.push_back(float(y + yOffset));
                }
            } catch (...) {
                continue;
            }
        }
    } else if (equalsPos != std::string::npos) {
        // Implicit function of x of the form f(x,y) = g(x,y)

        double x, y;

        symbol_table symbolTable;
        symbolTable.add_variable("x", x);
        symbolTable.add_variable("y", y);
        symbolTable.add_constants();
        addConstants(symbolTable);

        expression lhsExpression, rhsExpression;
        lhsExpression.register_symbol_table(symbolTable);
        rhsExpression.register_symbol_table(symbolTable);

        parser lhsParser, rhsParser;

        std::string lhs, rhs;

        lhs = functionStr.substr(0, equalsPos);
        rhs = functionStr.substr(equalsPos + 1);

        lhsParser.compile(lhs, lhsExpression);
        rhsParser.compile(rhs, rhsExpression);

        // Iterate over every pixel on the canvas and evaluate LHS and RHS at each pixel
        // If they are equal, add the pixel to the list of points
        for (int i = int(-xOffset); i <= canvasWidth - xOffset; i++) {
            for (int j = int(-yOffset); j <= canvasHeight - yOffset; j++) {
                try {
                    x = ((i - canvasWidth / 2) / canvasWidth) * xWidth;
                    y = (-(j - canvasHeight / 2) / canvasHeight) * yWidth;

                    double lhsVal = lhsExpression.value();
                    double rhsVal = rhsExpression.value();

                    if (approxEqual(lhsVal, rhsVal, 0.01) && !std::isnan(lhsVal) &&
                        !std::isnan(rhsVal)) {
                        points.push_back(float(i + xOffset));
                        points.push_back(float(j + yOffset));
                    }
                } catch (...) {
                    continue;
                }
            }
        }
    }

    while (points.size() > 100000) {
        removeAlternatingPairs(points);
    }

    jfloatArray jpoints = env->NewFloatArray(jsize(points.size()));
    if (jpoints != nullptr) {
        env->SetFloatArrayRegion(jpoints, 0, jsize(points.size()), points.data());
    }

    return jpoints;
}

std::string jstring2string(JNIEnv *env, jstring jStr) {
    jboolean isCopy;
    const char *convertedValue = (env)->GetStringUTFChars(jStr, &isCopy);
    std::string str = convertedValue;
    (env)->ReleaseStringUTFChars(jStr, convertedValue);
    return str;
}

template<typename T>
void removeAlternatingPairs(std::vector<T> &vec) {
    std::vector<T> result;
    long i = 0;
    long n = vec.size();

    while (i < n) {
        // Copy two elements (i.e., keep them)
        for (int j = 0; j < 2 && i < n; ++j, ++i) {
            result.push_back(vec[i]);
        }

        // Skip next two elements (i.e., remove them)
        i += 2;
    }

    vec = std::move(result); // Replace original vector with filtered one
}