package org.nsh07.simplygraph.ui

import android.annotation.SuppressLint
import androidx.compose.foundation.Canvas
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.material3.ExperimentalMaterial3ExpressiveApi
import androidx.compose.material3.MaterialTheme.colorScheme
import androidx.compose.material3.OutlinedTextField
import androidx.compose.material3.Scaffold
import androidx.compose.runtime.Composable
import androidx.compose.runtime.collectAsState
import androidx.compose.runtime.getValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clipToBounds
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.graphics.PointMode
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import androidx.lifecycle.viewmodel.compose.viewModel
import org.nsh07.simplygraph.ui.theme.SimplyGraphTheme
import org.nsh07.simplygraph.ui.viewModel.UiViewModel

@OptIn(ExperimentalMaterial3ExpressiveApi::class)
@SuppressLint("UnusedMaterial3ScaffoldPaddingParameter")
@Composable
fun AppScreen(modifier: Modifier = Modifier) {
    val viewModel: UiViewModel = viewModel()
    val graphState by viewModel.graphState.collectAsState()
    val functionsState by viewModel.functionsState.collectAsState()

    val colorScheme = colorScheme

    Scaffold(modifier = modifier) { insets ->
        Column {
            Canvas(
                Modifier
                    .clipToBounds()
                    .weight(1f)
                    .fillMaxWidth()
            ) {
                viewModel.updateCanvasSize(size)

                // Draw the x-axis
                drawLine(
                    color = colorScheme.onSurface,
                    start = Offset(0f, size.height / 2),
                    end = Offset(size.width, size.height / 2),
                    strokeWidth = 2.dp.toPx()
                )

                // Draw the y-axis
                drawLine(
                    color = colorScheme.onSurface,
                    start = Offset(size.width / 2, 0f),
                    end = Offset(size.width / 2, size.height),
                    strokeWidth = 2.dp.toPx()
                )

                // Draw the graph
                drawPoints(
                    graphState.points,
                    pointMode =
                        if (functionsState.function.contains('y'))
                            PointMode.Points
                        else PointMode.Polygon,
                    color = colorScheme.primary,
                    strokeWidth = 3.dp.toPx()
                )
            }
            Column(
                Modifier
                    .padding(16.dp)
                    .fillMaxWidth(),
                horizontalAlignment = Alignment.End
            ) {
                OutlinedTextField(
                    value = functionsState.function,
                    onValueChange = viewModel::updateFunction,
                    modifier = Modifier.fillMaxWidth()
                )
            }
            Spacer(Modifier.height(insets.calculateBottomPadding()))
        }
    }
}

@Preview
@Composable
fun AppScreenPreview() {
    SimplyGraphTheme {
        AppScreen(Modifier.fillMaxSize())
    }
}