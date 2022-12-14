package com.xinging.neontest;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.util.Log;

public class MainActivity extends AppCompatActivity {
    private static final String TAG = "MainActivity";
    static {
        System.loadLibrary("benchmark");
    }
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        float t = runBenchmark();
//        String x = getString();
//        Log.d(TAG, "onCreate: " + x);
    }

    private native float runBenchmark();
    private native String getString();
}