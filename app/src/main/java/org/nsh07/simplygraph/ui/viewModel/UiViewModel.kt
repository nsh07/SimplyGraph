package org.nsh07.simplygraph.ui.viewModel

import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.geometry.Size
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import kotlinx.coroutines.CancellationException
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.Job
import kotlinx.coroutines.delay
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
    private var updateJob: Job? = null

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

    fun updateOffset(offsetChange: Offset) {
        _graphState.update { currentState ->
            currentState.copy(
                xOffset = currentState.xOffset + offsetChange.x,
                yOffset = currentState.yOffset + offsetChange.y
            )
        }
        updateJob?.cancel()
        updateJob = viewModelScope.launch {
            delay(50)
            updateGraph()
        }
    }

    fun resetOffset() {
        _graphState.update { currentState ->
            currentState.copy(
                xOffset = 0f,
                yOffset = 0f
            )
        }
        updateGraph()
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

                    // We use the native C++ function for calculating graph points to plot faster

                    val newPoints = nativeBridge
                        .calculateGraphPoints(
                            xWidth = xWidth,
                            yWidth = yWidth.toDouble(),
                            xOffset = graphState.value.xOffset.toDouble(),
                            yOffset = graphState.value.yOffset.toDouble(),
                            canvasWidth = canvasSize.width.toDouble(),
                            canvasHeight = canvasSize.height.toDouble(),
                            function = function
                        )
                        .toOffsetList()

                    _graphState.update { currentState ->
                        currentState.copy(
                            invalidations = currentState.invalidations + 1,
                            points = newPoints
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

    fun FloatArray.toOffsetList(): List<Offset> {
        val size = this.size
        val result = ArrayList<Offset>(size / 2)
        var i = 0
        while (i + 1 < size) {
            result.add(Offset(this[i], this[i + 1]))
            i += 2
        }
        return result
    }
}