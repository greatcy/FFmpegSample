package com.eli.switchmp42yuv;

import android.os.Environment;
import android.os.Handler;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {

    private TextView mInputUrlTxt, mOutputUrlTxt;
    private Button mStartBtn, mCleanBtn;
    private ProgressBar mProgress;

    private static final String OUT_PATH = Environment.getExternalStorageDirectory().getAbsolutePath() + "/output.yuv";
    private static final String INPUT_PATH = Environment.getExternalStorageDirectory().getAbsolutePath() + "/sintel.mp4";
    private static final String TAG = MainActivity.class.getSimpleName();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        initView();

    }

    private void initView() {
        mInputUrlTxt = (TextView) findViewById(R.id.tv_input);
        mOutputUrlTxt = (TextView) findViewById(R.id.tv_output);

        mProgress = (ProgressBar) findViewById(R.id.progressBar);
        mStartBtn = (Button) findViewById(R.id.btn_start);
        mCleanBtn = (Button) findViewById(R.id.btn_clean);

        mStartBtn.setOnClickListener(this);
        mCleanBtn.setOnClickListener(this);

        mInputUrlTxt.setText(INPUT_PATH);
        mOutputUrlTxt.setText(OUT_PATH);
    }

    static {
        System.loadLibrary("ffmpeg-jni-lib");
    }

    public native int parseMP4Video(String inputUrl, String outputUrl);

    @Override
    public void onClick(View view) {
        if (view != null) {
            switch (view.getId()) {
                case R.id.btn_clean:
                    File outputFile = new File(OUT_PATH);
                    if (outputFile.exists() && outputFile.delete()) {
                        Toast.makeText(this, "The cache is delete " + OUT_PATH, Toast.LENGTH_LONG).show();
                    }
                    break;
                case R.id.btn_start:
                    File inputFile = new File(INPUT_PATH);
                    if (!inputFile.exists()) {
                        Toast.makeText(this, "The " + INPUT_PATH + " is empty", Toast.LENGTH_LONG).show();
                    } else {
                        Toast.makeText(MainActivity.this, "Start parseing...", Toast.LENGTH_LONG).show();
                        new Thread(new Runnable() {
                            @Override
                            public void run() {
                                parseMP4Video(INPUT_PATH, OUT_PATH);
                                new Handler(getMainLooper()).post(new Runnable() {
                                    @Override
                                    public void run() {
                                        Toast.makeText(MainActivity.this, "parse video done!", Toast.LENGTH_LONG).show();
                                    }
                                });
                            }
                        }).start();
                    }
                    break;
            }
        }
    }
}