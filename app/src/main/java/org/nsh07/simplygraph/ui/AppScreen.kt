package org.nsh07.simplygraph.ui

import android.annotation.SuppressLint
import androidx.compose.foundation.Canvas
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.material3.Button
import androidx.compose.material3.ButtonDefaults
import androidx.compose.material3.ExperimentalMaterial3ExpressiveApi
import androidx.compose.material3.MaterialTheme.colorScheme
import androidx.compose.material3.OutlinedTextField
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableIntStateOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.geometry.Size
import androidx.compose.ui.graphics.PointMode
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import net.objecthunter.exp4j.ExpressionBuilder
import org.nsh07.simplygraph.ui.theme.SimplyGraphTheme

@OptIn(ExperimentalMaterial3ExpressiveApi::class)
@SuppressLint("UnusedMaterial3ScaffoldPaddingParameter")
@Composable
fun AppScreen(modifier: Modifier = Modifier) {
    val xWidth = 10 // Number of integral x-coordinates visible on screen
    val colorScheme = colorScheme
    var function by remember { mutableStateOf("x") }
    var canvasSize by remember { mutableStateOf(Size(0f, 0f)) }
    var points = emptyList<Offset>()
    var invalidations by remember { mutableIntStateOf(0) }
    var redrawCount by remember { mutableIntStateOf(0) }

    LaunchedEffect(redrawCount) {
        // Evaluate expression at various points along the x-axis and find y-values accordingly
        if (function.isNotEmpty()) {
            try {
                val exp = ExpressionBuilder(function)
                    .variable("x")
                    .build()

                val newPoints = mutableListOf<Offset>()
                val widthInt = canvasSize.width.toInt()

                for (i in 0..widthInt) {
                    // Calculate the value of x and y for the given i-pixel-offset
                    // x can be simply calculated by subtracting half of canvas width (to shift origin to half of the screen) and then scaling according to xWidth
                    val x = ((i.toDouble() - canvasSize.width / 2) / canvasSize.width) * xWidth

                    // Calculating y is a bit more complex, we find the value of f(x) for x as calculated above,
                    // then multiply it by the below factor to scale it to keep the x and y-axis scale uniform in the graph
                    // We then subtract this value from half of the canvas width (to shift origin to half the screen)
                    // And voila, we get a graph with the origin at the center of the screen.
                    val y =
                        (-exp.setVariable("x", x).evaluate() * (canvasSize.height / (xWidth * (canvasSize.height / canvasSize.width)))) + canvasSize.height / 2
                    newPoints.add(Offset(i.toFloat(), y.toFloat()))
                }

                points = newPoints.toList()
                invalidations++ // Forces a redraw (recomposition) of the canvas
            } catch (_: Exception) {
                points = emptyList()
                invalidations++
            }
        }
    }

    Scaffold(modifier = modifier) { insets ->
        Column {
            Canvas(
                Modifier
                    .weight(1f)
                    .fillMaxWidth()
            ) {
                invalidations
                canvasSize = size

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

                // Draw gridlines
                for (i in 0..xWidth) {
                    val x = (i.toDouble() - xWidth / 2) * (size.width / xWidth) + size.width / 2
                    drawLine(
                        color = colorScheme.surfaceDim,
                        start = Offset(x.toFloat(), 0f),
                        end = Offset(x.toFloat(), size.height)
                    )
                }

                for (i in 0..(xWidth * (canvasSize.height / canvasSize.width)).toInt()) {
                    val y = (i.toDouble() - xWidth * (canvasSize.height / canvasSize.width) / 2) * (size.height / (xWidth * (canvasSize.height / canvasSize.width))) + size.height / 2
                    drawLine(
                        color = colorScheme.surfaceDim,
                        start = Offset(0f, y.toFloat()),
                        end = Offset(size.width, y.toFloat())
                    )
                }

                // Draw the graph

                drawPoints(
                    points,
                    pointMode = PointMode.Polygon,
                    color = colorScheme.primary,
                    strokeWidth = 4.dp.toPx()
                )
            }
            Column(Modifier.padding(32.dp)) {
                OutlinedTextField(
                    value = function,
                    onValueChange = { function = it }
                )
                Spacer(Modifier.height(8.dp))
                Button(onClick = { redrawCount++ }, shapes = ButtonDefaults.shapes()) {
                    Text("Redraw")
                }
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