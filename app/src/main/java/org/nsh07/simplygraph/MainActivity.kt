package org.nsh07.simplygraph

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.ui.Modifier
import org.nsh07.simplygraph.ui.AppScreen
import org.nsh07.simplygraph.ui.theme.SimplyGraphTheme

class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        setContent {
            SimplyGraphTheme {
                AppScreen(Modifier.fillMaxSize())
            }
        }
    }
}