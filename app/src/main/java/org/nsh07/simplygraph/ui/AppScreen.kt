package org.nsh07.simplygraph.ui

import android.annotation.SuppressLint
import androidx.compose.animation.AnimatedContent
import androidx.compose.animation.AnimatedVisibility
import androidx.compose.animation.core.animateDpAsState
import androidx.compose.foundation.Canvas
import androidx.compose.foundation.gestures.rememberTransformableState
import androidx.compose.foundation.gestures.transformable
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.WindowInsets
import androidx.compose.foundation.layout.asPaddingValues
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.systemBars
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.text.KeyboardOptions
import androidx.compose.material3.BottomSheetDefaults
import androidx.compose.material3.BottomSheetScaffold
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.ExperimentalMaterial3ExpressiveApi
import androidx.compose.material3.FloatingActionButton
import androidx.compose.material3.Icon
import androidx.compose.material3.LocalTextStyle
import androidx.compose.material3.MaterialTheme.colorScheme
import androidx.compose.material3.MaterialTheme.motionScheme
import androidx.compose.material3.MaterialTheme.shapes
import androidx.compose.material3.MaterialTheme.typography
import androidx.compose.material3.OutlinedTextField
import androidx.compose.material3.SheetValue
import androidx.compose.material3.Switch
import androidx.compose.material3.SwitchDefaults
import androidx.compose.material3.Text
import androidx.compose.material3.animateFloatingActionButton
import androidx.compose.material3.rememberBottomSheetScaffoldState
import androidx.compose.runtime.Composable
import androidx.compose.runtime.collectAsState
import androidx.compose.runtime.getValue
import androidx.compose.runtime.remember
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clipToBounds
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.graphics.PointMode
import androidx.compose.ui.graphics.StrokeCap
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.text.input.ImeAction
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import androidx.lifecycle.viewmodel.compose.viewModel
import org.nsh07.simplygraph.R
import org.nsh07.simplygraph.ui.theme.SimplyGraphTheme
import org.nsh07.simplygraph.ui.viewModel.UiViewModel

@OptIn(ExperimentalMaterial3ExpressiveApi::class, ExperimentalMaterial3Api::class)
@SuppressLint("UnusedMaterial3ScaffoldPaddingParameter")
@Composable
fun AppScreen(modifier: Modifier = Modifier) {
    val viewModel: UiViewModel = viewModel()
    val graphState by viewModel.graphState.collectAsState()
    val functionsState by viewModel.functionsState.collectAsState()

    val expandSheet = remember(functionsState.function) {
        functionsState.function.matches(
            "\\s*r\\s*=.+|\\s*\\(.+,.+\\)\\s*".toRegex()
        )
    }

    val colorScheme = colorScheme
    val transformableState = rememberTransformableState { _, offsetChange, _ ->
        viewModel.updateOffset(offsetChange)
    }
    val scaffoldState = rememberBottomSheetScaffoldState()
    val topSpacing by animateDpAsState(
        if (scaffoldState.bottomSheetState.targetValue == SheetValue.Expanded)
            WindowInsets.systemBars.asPaddingValues().calculateTopPadding()
        else 0.dp,
        animationSpec = motionScheme.defaultSpatialSpec()
    )
    val bottomSpacing by animateDpAsState(
        if (
            scaffoldState.bottomSheetState.targetValue == SheetValue.PartiallyExpanded &&
            !expandSheet
        )
            WindowInsets.systemBars.asPaddingValues().calculateBottomPadding() + 16.dp
        else 16.dp,
        animationSpec = motionScheme.defaultSpatialSpec()
    )
    val sheetPeekHeight by animateDpAsState(
        if (expandSheet) 164.dp
        else 64.dp,
        animationSpec = motionScheme.defaultSpatialSpec()
    )

    BottomSheetScaffold(
        scaffoldState = scaffoldState,
        sheetContent = {
            Column(
                Modifier
                    .padding(16.dp)
                    .fillMaxSize()
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
                AnimatedVisibility(expandSheet) {
                    AnimatedContent(functionsState.function.contains(',')) {
                        when (it) {
                            true -> {
                                Row {
                                    Column {
                                        Spacer(Modifier.height(bottomSpacing))
                                        Text(
                                            "Parameter interval",
                                            style = typography.titleSmall,
                                            modifier = Modifier.padding(bottom = 8.dp, start = 4.dp)
                                        )
                                        Row {
                                            OutlinedTextField(
                                                value = functionsState.tStart,
                                                textStyle = LocalTextStyle.current.copy(textAlign = TextAlign.End),
                                                suffix = { Text(" ⩽ t") },
                                                onValueChange = { viewModel.updateTInterval(start = it) },
                                                keyboardOptions = KeyboardOptions(imeAction = ImeAction.Next),
                                                shape = shapes.large,
                                                modifier = Modifier.width(128.dp)
                                            )
                                            Spacer(Modifier.width(8.dp))
                                            OutlinedTextField(
                                                value = functionsState.tEnd,
                                                textStyle = LocalTextStyle.current.copy(textAlign = TextAlign.Start),
                                                prefix = { Text("t ⩽ ") },
                                                onValueChange = { viewModel.updateTInterval(end = it) },
                                                keyboardOptions = KeyboardOptions(imeAction = ImeAction.Done),
                                                shape = shapes.large,
                                                modifier = Modifier.width(128.dp)
                                            )
                                        }
                                    }
                                    Column(
                                        horizontalAlignment = Alignment.CenterHorizontally,
                                        modifier = Modifier.padding(start = 8.dp)
                                    ) {
                                        Spacer(Modifier.height(bottomSpacing))
                                        Text(
                                            "Connect points",
                                            style = typography.titleSmall,
                                            modifier = Modifier.padding(bottom = 8.dp, start = 4.dp)
                                        )
                                        Switch(
                                            checked = graphState.connectPoints,
                                            onCheckedChange = viewModel::setConnectPoints,
                                            thumbContent = {
                                                if (graphState.connectPoints) {
                                                    Icon(
                                                        painterResource(R.drawable.check),
                                                        contentDescription = null,
                                                        modifier = Modifier.size(SwitchDefaults.IconSize),
                                                    )
                                                }
                                            }
                                        )
                                    }
                                }
                            }

                            else -> {
                                Row {
                                    Column {
                                        Spacer(Modifier.height(bottomSpacing))
                                        Text(
                                            "Theta (angle) interval",
                                            style = typography.titleSmall,
                                            modifier = Modifier.padding(bottom = 8.dp, start = 4.dp)
                                        )
                                        Row {
                                            OutlinedTextField(
                                                value = functionsState.thetaStart,
                                                textStyle = LocalTextStyle.current.copy(textAlign = TextAlign.End),
                                                suffix = { Text(" ⩽ θ") },
                                                onValueChange = {
                                                    viewModel.updateThetaInterval(
                                                        start = it
                                                    )
                                                },
                                                keyboardOptions = KeyboardOptions(imeAction = ImeAction.Next),
                                                shape = shapes.large,
                                                modifier = Modifier.width(128.dp)
                                            )
                                            Spacer(Modifier.width(8.dp))
                                            OutlinedTextField(
                                                value = functionsState.thetaEnd,
                                                textStyle = LocalTextStyle.current.copy(textAlign = TextAlign.Start),
                                                prefix = { Text("θ ⩽ ") },
                                                onValueChange = { viewModel.updateThetaInterval(end = it) },
                                                keyboardOptions = KeyboardOptions(imeAction = ImeAction.Done),
                                                shape = shapes.large,
                                                modifier = Modifier.width(128.dp)
                                            )
                                        }
                                    }
                                    Column(
                                        horizontalAlignment = Alignment.CenterHorizontally,
                                        modifier = Modifier.padding(start = 8.dp)
                                    ) {
                                        Spacer(Modifier.height(bottomSpacing))
                                        Text(
                                            "Connect points",
                                            style = typography.titleSmall,
                                            modifier = Modifier.padding(bottom = 8.dp, start = 4.dp)
                                        )
                                        Switch(
                                            checked = graphState.connectPoints,
                                            onCheckedChange = viewModel::setConnectPoints,
                                            thumbContent = {
                                                if (graphState.connectPoints) {
                                                    Icon(
                                                        painterResource(R.drawable.check),
                                                        contentDescription = null,
                                                        modifier = Modifier.size(SwitchDefaults.IconSize),
                                                    )
                                                }
                                            }
                                        )
                                    }
                                }
                            }
                        }
                    }
                }
            }
        },
        sheetDragHandle = null,
        sheetPeekHeight = BottomSheetDefaults.SheetPeekHeight + sheetPeekHeight,
        modifier = modifier
    ) { insets ->
        Box {
            Canvas(
                Modifier
                    .transformable(transformableState)
                    .clipToBounds()
                    .fillMaxSize()
                    .padding(bottom = insets.calculateBottomPadding())
            ) {
                if (graphState.canvasSize != size)
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
                drawPoints(
                    graphState.points,
                    pointMode =
                        if (functionsState.function.contains('y') || !graphState.connectPoints)
                            PointMode.Points
                        else PointMode.Polygon,
                    color = colorScheme.primary,
                    strokeWidth = 3.dp.toPx(),
                    cap = StrokeCap.Round
                )
            }
            FloatingActionButton(
                onClick = viewModel::resetOffset,
                modifier = Modifier
                    .align(Alignment.BottomEnd)
                    .padding(bottom = insets.calculateBottomPadding())
                    .padding(16.dp)
                    .animateFloatingActionButton(
                        graphState.xOffset != 0f || graphState.yOffset != 0f,
                        Alignment.BottomEnd
                    )
            ) {
                Icon(painterResource(R.drawable.origin), contentDescription = "Reset to origin")
            }
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