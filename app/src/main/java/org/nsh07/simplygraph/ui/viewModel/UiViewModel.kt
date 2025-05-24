package org.nsh07.simplygraph.ui.viewModel

import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.geometry.Size
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import kotlinx.coroutines.CancellationException
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.Job
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.flow.update
import kotlinx.coroutines.launch
import org.nsh07.simplygraph.NativeBridge


class UiViewModel : ViewModel() {
    private val _graphState = MutableStateFlow(GraphState())
    val graphState: StateFlow<GraphState> = _graphState.asStateFlow()
    private val _functionsState = MutableStateFlow(FunctionsState(""))
    val functionsState: StateFlow<FunctionsState> = _functionsState.asStateFlow()

    private var nativeBridge = NativeBridge()
    private var calculationJob: Job? = null

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
            calculationJob?.cancel()
            calculationJob = viewModelScope.launch(Dispatchers.IO) {
                try {
                    val xWidth =
                        graphState.value.xWidth // Number of integral x-coordinates visible on screen
                    val yWidth =
                        xWidth * (graphState.value.canvasSize.height / graphState.value.canvasSize.width)
                    val function = functionsState.value.function
                    val canvasSize = graphState.value.canvasSize

                    val hasY = function.contains('y')
                    val hasX = function.contains('x')

                    val newPoints = mutableListOf<Offset>()

                    // We use the native C++ function for calculating graph points to plot faster
                    val graphPoints = nativeBridge.calculateGraphPoints(
                        xWidth = xWidth,
                        yWidth = yWidth.toDouble(),
                        canvasWidth = canvasSize.width.toDouble(),
                        canvasHeight = canvasSize.height.toDouble(),
                        function = function,
                        hasY = hasY,
                        hasX = hasX
                    )
                    newPoints.addAll(
                        graphPoints
                            .toList()
                            .chunked(2)
                            .map { (x, y) ->
                                Offset(x, y)
                            }
                    )

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
}