package org.nsh07.simplygraph

class NativeBridge {
    init {
        System.loadLibrary("native-lib")
    }

    external fun calculateGraphPoints(
        xWidth: Int,
        yWidth: Double,
        canvasWidth: Double,
        canvasHeight: Double,
        function: String
    ): FloatArray
}