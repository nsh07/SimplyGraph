//
// Created by nishant on 24/05/25.
//

#include <jni.h>

#include <cmath>
#include <string>
#include <vector>

#include "exprtk.hpp"
#include "funcs.h"

typedef exprtk::symbol_table<double> symbol_table;
typedef exprtk::expression<double> expression;
typedef exprtk::parser<double> parser;

extern "C"
JNIEXPORT jfloatArray JNICALL
Java_org_nsh07_simplygraph_NativeBridge_calculateGraphPoints(
        JNIEnv *env,
        jobject,
        jint xWidth,
        jdouble yWidth,
        jdouble canvasWidth,
        jdouble canvasHeight,
        jstring function
) {
    double yScaleFactor = canvasHeight / yWidth;
    std::vector<float> points;

    jboolean isCopy;
    const char *convertedValue = (env)->GetStringUTFChars(function, &isCopy);
    std::string functionStr = convertedValue;
    (env)->ReleaseStringUTFChars(function, convertedValue);

    bool hasX = functionStr.find('x') != std::string::npos;
    bool hasY = functionStr.find('y') != std::string::npos;

    if (!hasY) {
        double x;

        symbol_table symbolTable;
        symbolTable.add_variable("x", x);
        symbolTable.add_constants();

        expression expression;
        expression.register_symbol_table(symbolTable);

        parser parser;
        parser.compile(functionStr, expression);

        for (int i = 0; i <= canvasWidth; i++) {
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
                    points.push_back(float(i));
                    points.push_back(float(y));
                }
            } catch (...) {
                continue;
            }
        }
    } else if (hasX) {
        double x, y;

        symbol_table symbolTable;
        symbolTable.add_variable("x", x);
        symbolTable.add_variable("y", y);
        symbolTable.add_constants();

        expression lhsExpression, rhsExpression;
        lhsExpression.register_symbol_table(symbolTable);
        rhsExpression.register_symbol_table(symbolTable);

        parser lhsParser, rhsParser;

        auto equalsIndex = functionStr.find('=');
        std::string lhs, rhs;

        if (equalsIndex != std::string::npos) {
            lhs = functionStr.substr(0, equalsIndex - 1);
            rhs = functionStr.substr(equalsIndex + 1);
        } else {
            lhs = "y";
            rhs = functionStr;
        }

        lhsParser.compile(lhs, lhsExpression);
        rhsParser.compile(rhs, rhsExpression);

        // Iterate over every pixel on the canvas and evaluate LHS and RHS at each pixel
        // If they are equal, add the pixel to the list of points
        for (int i = 0; i <= canvasWidth; i++) {
            for (int j = 0; j <= canvasHeight; j++) {
                try {
                    x = ((i - canvasWidth / 2) / canvasWidth) * xWidth;
                    y = (-(j - canvasHeight / 2) / canvasHeight) * yWidth;

                    double lhsVal = lhsExpression.value();
                    double rhsVal = rhsExpression.value();

                    if (approxEqual(lhsVal, rhsVal, 0.01) && !std::isnan(lhsVal) && !std::isnan(rhsVal)) {
                        points.push_back(float(i));
                        points.push_back(float(j));
                    }
                } catch (...) {
                    continue;
                }
            }
        }
    }

    jfloatArray jpoints = env->NewFloatArray(jsize(points.size()));
    if (jpoints != nullptr) {
        env->SetFloatArrayRegion(jpoints, 0, jsize(points.size()), points.data());
    }

    return jpoints;
}