package org.nsh07.simplygraph.ui

import android.annotation.SuppressLint
import androidx.compose.animation.core.animateDpAsState
import androidx.compose.foundation.Canvas
import androidx.compose.foundation.gestures.rememberTransformableState
import androidx.compose.foundation.gestures.transformable
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.WindowInsets
import androidx.compose.foundation.layout.asPaddingValues
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.systemBars
import androidx.compose.foundation.text.KeyboardOptions
import androidx.compose.material3.BottomSheetDefaults
import androidx.compose.material3.BottomSheetScaffold
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.ExperimentalMaterial3ExpressiveApi
import androidx.compose.material3.MaterialTheme.colorScheme
import androidx.compose.material3.MaterialTheme.shapes
import androidx.compose.material3.OutlinedTextField
import androidx.compose.material3.SheetValue
import androidx.compose.material3.Text
import androidx.compose.material3.rememberBottomSheetScaffoldState
import androidx.compose.runtime.Composable
import androidx.compose.runtime.collectAsState
import androidx.compose.runtime.getValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clipToBounds
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.graphics.PointMode
import androidx.compose.ui.text.input.ImeAction
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import androidx.lifecycle.viewmodel.compose.viewModel
import org.nsh07.simplygraph.ui.theme.SimplyGraphTheme
import org.nsh07.simplygraph.ui.viewModel.UiViewModel

@OptIn(ExperimentalMaterial3ExpressiveApi::class, ExperimentalMaterial3Api::class)
@SuppressLint("UnusedMaterial3ScaffoldPaddingParameter")
@Composable
fun AppScreen(modifier: Modifier = Modifier) {
    val viewModel: UiViewModel = viewModel()
    val graphState by viewModel.graphState.collectAsState()
    val functionsState by viewModel.functionsState.collectAsState()

    val colorScheme = colorScheme
    val transformableState = rememberTransformableState { _, offsetChange, _ ->
        viewModel.updateOffset(offsetChange)
    }
    val scaffolState = rememberBottomSheetScaffoldState()
    val topSpacing by animateDpAsState(
        if (scaffolState.bottomSheetState.targetValue == SheetValue.Expanded)
            WindowInsets.systemBars.asPaddingValues().calculateTopPadding()
        else 0.dp
    )

    BottomSheetScaffold(
        scaffoldState = scaffolState,
        sheetContent = {
            Column(
                Modifier
                    .padding(16.dp)
                    .fillMaxSize(),
                horizontalAlignment = Alignment.End
            ) {
                Spacer(Modifier.height(topSpacing))
                OutlinedTextField(
                    value = functionsState.function,
                    onValueChange = viewModel::updateFunction,
                    label = { Text("Function") },
                    placeholder = { Text("f(x) | f(x, y) = g(x, y) | r = f(theta)") },
                    keyboardOptions = KeyboardOptions(imeAction = ImeAction.Done),
                    shape = shapes.large,
                    modifier = Modifier.fillMaxWidth()
                )
                Spacer(
                    Modifier.height(
                        WindowInsets.systemBars.asPaddingValues().calculateBottomPadding()
                    )
                )
            }
        },
        sheetDragHandle = null,
        sheetPeekHeight = BottomSheetDefaults.SheetPeekHeight + 64.dp,
        modifier = modifier
    ) { insets ->
        Canvas(
            Modifier
                .transformable(transformableState)
                .clipToBounds()
                .fillMaxSize()
                .padding(bottom = insets.calculateBottomPadding())
        ) {
            viewModel.updateCanvasSize(size)

            // Draw the x-axis
            drawLine(
                color = colorScheme.onSurface,
                start = Offset(0f, size.height / 2 + graphState.yOffset),
                end = Offset(size.width, size.height / 2 + graphState.yOffset),
                strokeWidth = 2.dp.toPx()
            )

            // Draw the y-axis
            drawLine(
                color = colorScheme.onSurface,
                start = Offset(size.width / 2 + graphState.xOffset, 0f),
                end = Offset(size.width / 2 + graphState.xOffset, size.height),
                strokeWidth = 2.dp.toPx()
            )

            // Draw the graph
            if (graphState.points.size < 1000000) // Avoids out of memory errors in very dense graphs
                drawPoints(
                    graphState.points,
                    pointMode =
                        if (functionsState.function.contains('='))
                            PointMode.Points
                        else PointMode.Polygon,
                    color = colorScheme.primary,
                    strokeWidth = 3.dp.toPx()
                )
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