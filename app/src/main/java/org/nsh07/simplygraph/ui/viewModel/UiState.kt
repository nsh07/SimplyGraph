package org.nsh07.simplygraph.ui.viewModel

import androidx.compose.runtime.Immutable
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.geometry.Size

@Immutable
data class GraphState(
    val invalidations: Int = 0,
    val canvasSize: Size = Size(0f, 0f),
    val points: List<Offset> = emptyList(),
    val xWidth: Float = 10f,
    val xOffset: Float = 0f,
    val yOffset: Float = 0f,
    val connectPoints: Boolean = true
)

@Immutable
data class FunctionsState(
    val function: String,
    val tStart: String = "0",
    val tEnd: String = "1",
    val thetaStart: String = "0",
    val thetaEnd: String = "12pi"
)