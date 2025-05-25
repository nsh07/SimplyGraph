//
// Created by nishant on 24/05/25.
//

#include <jni.h>

#include <cmath>
#include <string>
#include <vector>

#include "exprtk.hpp"
#include "funcs.hpp"

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
    double xScaleFactor = canvasWidth / xWidth;
    double yScaleFactor = canvasHeight / yWidth;
    std::vector<float> points;

    jboolean isCopy;
    const char *convertedValue = (env)->GetStringUTFChars(function, &isCopy);
    std::string functionStr = convertedValue;
    (env)->ReleaseStringUTFChars(function, convertedValue);

    auto equalsIndex = functionStr.find('=');
    bool hasX = functionStr.find('x') != std::string::npos;
    bool hasY = functionStr.find('y') != std::string::npos;

    std::string preEqual = functionStr.substr(0, equalsIndex - 1);
    bool isPolar = (equalsIndex != std::string::npos) && (trim(preEqual) == "r");

    if (isPolar) {
        /*
        Polar equation
        This is very easy to draw, the polar equation is of the form r = f(theta), we iterate over
        various values of theta from 0 to 12pi, calculate r from the above equation, and add the point
        (r * cos(theta), r * sin(theta)) to the graph (since x = r cos(theta) and y = r sin(theta))
        */
        double r, theta = 0.0;
        std::string rhs = functionStr.substr(equalsIndex + 1);

        symbol_table symbolTable;
        symbolTable.add_variable("theta", theta);
        symbolTable.add_constants();

        expression expression;
        expression.register_symbol_table(symbolTable);

        parser parser;
        parser.compile(rhs, expression);

        while (theta < 12 * M_PI) {
            r = expression.value();
            if (!std::isnan(r)) {
                points.push_back(float(r * cos(theta) * xScaleFactor + canvasWidth / 2));
                points.push_back(float(-r * sin(theta) * yScaleFactor + canvasHeight / 2));
            }
            theta += 0.1;
        }
    } else if (!hasY) {
        // Explicit function of x (in the form f(x))
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
        // Implicit function of x,y of the form f(x,y) = g(x,y) or f(x,y)
        // (equality with y is implied in the second case)
        double x, y;

        symbol_table symbolTable;
        symbolTable.add_variable("x", x);
        symbolTable.add_variable("y", y);
        symbolTable.add_constants();

        expression lhsExpression, rhsExpression;
        lhsExpression.register_symbol_table(symbolTable);
        rhsExpression.register_symbol_table(symbolTable);

        parser lhsParser, rhsParser;

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

                    if (approxEqual(lhsVal, rhsVal, 0.01) && !std::isnan(lhsVal) &&
                        !std::isnan(rhsVal)) {
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