package org.nsh07.simplygraph

class NativeBridge {
    init {
        System.loadLibrary("native-lib")
    }

    external fun calculateGraphPoints(
        xWidth: Int,
        yWidth: Double,
        xOffset: Double,
        yOffset: Double,
        canvasWidth: Double,
        canvasHeight: Double,
        tStart: String,
        tEnd: String,
        thetaStart: String,
        thetaEnd: String,
        function: String
    ): FloatArray
}