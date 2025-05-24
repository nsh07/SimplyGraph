package org.nsh07.simplygraph.ui.viewModel

import android.R.attr.x
import android.util.Log
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.geometry.Size
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import kotlinx.coroutines.CancellationException
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.Job
import kotlinx.coroutines.ensureActive
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.flow.update
import kotlinx.coroutines.launch
import net.objecthunter.exp4j.ExpressionBuilder
import kotlin.math.abs

class UiViewModel : ViewModel() {
    private val _graphState = MutableStateFlow(GraphState())
    val graphState: StateFlow<GraphState> = _graphState.asStateFlow()
    private val _functionsState = MutableStateFlow(FunctionsState(""))
    val functionsState: StateFlow<FunctionsState> = _functionsState.asStateFlow()

    private var calculationJob = Job()
        get() {
            if (field.isCancelled) field = Job()
            return field
        }

    fun updateFunction(function: String) {
        _functionsState.update { currentState ->
            currentState.copy(
                function = function
            )
        }
        updateGraph()
    }

    fun updateCanvasSize(size: Size) {
        _graphState.update { currentState ->
            currentState.copy(
                canvasSize = size
            )
        }
    }

    fun updateGraph() {
        if (functionsState.value.function.isNotEmpty()) {
            calculationJob.cancel()
            viewModelScope.launch(Dispatchers.IO + calculationJob) {
                try {
                    val xWidth =
                        graphState.value.xWidth // Number of integral x-coordinates visible on screen
                    val yWidth =
                        xWidth * (graphState.value.canvasSize.height / graphState.value.canvasSize.width)
                    val function = functionsState.value.function
                    val canvasSize = graphState.value.canvasSize

                    val hasY = function.contains('y')

                    val exp =
                        // If the function does not contain y (is an explicit function of x)
                        if (!hasY) {
                            ExpressionBuilder(function)
                                .variable("x")
                                .build()
                        } else null

                    val expRhs =
                        if (hasY) {
                            // If the function also contains y, we need to split the RHS and LHS and evaluate them separately
                            // This contains the RHS
                            val rhs = function.substringAfter('=')
                            ExpressionBuilder(rhs)
                                .variable("x")
                                .variable("y")
                                .build()
                        } else null

                    val expLhs = if (hasY) {
                        // LHS
                        val lhs = function.substringBefore('=', "y")
                        ExpressionBuilder(lhs)
                            .variable("x")
                            .variable("y")
                            .build()
                    } else null

                    val res = exp?.validate(false)
                    val resRhs = expRhs?.validate(false)
                    val resLhs = expLhs?.validate(false)

                    if (!hasY) {
                        if (res?.isValid != true) {
                            Log.e("Graph", "Invalid function: $function")
                            throw IllegalArgumentException()
                        }
                    } else {
                        if (resRhs?.isValid != true || resLhs?.isValid != true ||
                            !function.contains('x')
                        ) {
                            Log.e("Graph", "Invalid function: $function\n${resRhs?.errors}")
                            throw IllegalArgumentException()
                        }
                    }

                    val newPoints = mutableListOf<Offset>()
                    val widthInt = canvasSize.width.toInt()
                    val heightInt = canvasSize.height.toInt()
                    val yScaleFactor =
                        (canvasSize.height / (yWidth))

                    if (!hasY) {
                        for (i in 0..widthInt) {
                            ensureActive()
                            try {
                                // Calculate the value of x and y for the given i-pixel-offset
                                // x can be simply calculated by subtracting half of canvas width (to shift origin to half of the screen) and then scaling according to xWidth
                                val x =
                                    ((i.toDouble() - canvasSize.width / 2) / canvasSize.width) * xWidth

                                // Calculating y is a bit more complex, we find the value of f(x) for x as calculated above,
                                // then multiply it by the below factor to scale it to keep the x and y-axis scale uniform in the graph
                                // We then subtract this value from half of the canvas width (to shift origin to half the screen)
                                // And voila, we get a graph with the origin at the center of the screen.
                                val y = (-exp!!.setVariable("x", x).evaluate() * yScaleFactor) +
                                        canvasSize.height / 2
                                newPoints.add(Offset(i.toFloat(), y.toFloat()))
                            } catch (_: Exception) {
                                Log.e(
                                    "Graph",
                                    "Cannot evaluate point:\nx = $x\nfunction = $function"
                                )
                            }
                        }
                    } else {
                        // We iterate over the entire canvas pixel-by-pixel and check whether the function holds
                        // at each pixel
                        for (i in 0..widthInt) {
                            for (j in 0..heightInt) {
                                ensureActive()
                                try {
                                    val x =
                                        ((i.toDouble() - canvasSize.width / 2) / canvasSize.width) * xWidth
                                    val y =
                                        (-(j.toDouble() - canvasSize.height / 2) / canvasSize.height) * yWidth
                                    val varMap = mapOf("x" to x, "y" to y)

                                    val rhs = expRhs!!.setVariables(varMap).evaluate()
                                    val lhs = expLhs!!.setVariables(varMap).evaluate()

                                    if (approxEqual(lhs, rhs, 0.01)) {
                                        // Consider LHS and RHS to be equal when they are within 1% of LHS
                                        newPoints.add(Offset(i.toFloat(), j.toFloat()))
                                    }
                                } catch (_: Exception) {
                                    Log.e(
                                        "Graph",
                                        "Cannot evaluate point:\nfunction = $function"
                                    )
                                }
                            }
                        }
                    }

                    _graphState.update { currentState ->
                        currentState.copy(
                            invalidations = currentState.invalidations + 1,
                            points = newPoints.toList()
                        )
                    }
                } catch (e: Exception) {
                    if (e !is CancellationException)
                        _graphState.update { currentState ->
                        currentState.copy(
                            invalidations = currentState.invalidations + 1,
                            points = emptyList()
                        )
                    }
                }
            }
        }
    }

    private fun approxEqual(a: Double, b: Double, eps: Double): Boolean {
        return if (a != 0.0) abs((a - b) / a) < eps
        else a == b
    }
}