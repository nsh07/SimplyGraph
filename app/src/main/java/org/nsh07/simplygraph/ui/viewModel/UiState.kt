package org.nsh07.simplygraph.ui.viewModel

import androidx.compose.runtime.Immutable
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.geometry.Size

@Immutable
data class GraphState(
    val invalidations: Int = 0,
    val canvasSize: Size = Size(0f, 0f),
    val points: List<Offset> = emptyList(),
    val xWidth: Int = 10,
    val xOffset: Float = 0f,
    val yOffset: Float = 0f
)

@Immutable
data class FunctionsState(
    val function: String,
    val tStart: String = "0",
    val tEnd: String = "1"
)