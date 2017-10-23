## Tilt Shift

Tilt shifting consists of blurring the boundaries for selective focus, often producing miniature effect. To achieve this, element-wise multiplication is done with masks of hyperbolic tangents vertically. For example,

![Equation](./results/equation_tanh.png)

and two representations of the image: focused and blurred. Afterwards, these two results are added to produce the final image, that has focus on the center and blurred at the boundaries.

![Tilt shift](./results/result_tiltshift.gif)

The parameters such start of focus, decay and center of focus are controlled using GUI elements of OpenCV. Changing these values reflect in the equation for the vertical brightness of the masks.

To use this program, run `./tiltshift <img>`.

#### tiltshift.cpp
```c++
#include <iostream>
#include <cmath>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

double start_focus = 20;
int start_focus_slider = 20;
int start_focus_slider_max = 100;

double decay_strength = 100;
int decay_strength_slider = 100;
int decay_strength_slider_max = 100;

double center_focus = 50;
int center_focus_slider = 50;
int center_focus_slider_max = 100;

int hue_gain = 0;
int hue_gain_slider = 0;
int hue_gain_slider_max = 255;

Mat image, temp_image, blurred_image;
Mat func_image, compl_image;
Mat m_image, m_bimage;
Mat result;

char TrackbarName[50];

void drawFuncImage() {
    uchar pixel_value;
    double x;
    double den = (decay_strength > 0 ? decay_strength/10 : 0.1);
    double func_val;
    for (int i = 0; i < func_image.rows; ++i) {
        x = (double) i*100.0 /func_image.rows;
        func_val =
            ( tanh( (x-start_focus)/den ) - 
              tanh ( ( x-(2*center_focus-start_focus) )/den ) ) / 2;
        pixel_value = (uchar) (255*func_val);
        for (int j = 0; j < func_image.cols; ++j) {
            func_image.at<uchar>(i,j) = pixel_value;
            compl_image.at<uchar>(i,j) = 255 - pixel_value;
        }
    }
    imshow( "func_image",  func_image);
}

void composeResult() {
    drawFuncImage();
    
    Mat image_f, blurred_image_f;
    image.convertTo(image_f, CV_32F);
    blurred_image.convertTo(blurred_image_f, CV_32F);

    Mat func_image3, compl_image3;
    
    Mat t_func[]  = { func_image,  func_image,  func_image};
    Mat t_compl[] = {compl_image, compl_image, compl_image};

    merge( t_func, 3,  func_image3);
    merge(t_compl, 3, compl_image3);

    Mat func_image3_f, compl_image3_f;
    func_image3.convertTo(func_image3_f, CV_32F, 1.0/255.0);
    compl_image3.convertTo(compl_image3_f, CV_32F, 1.0/255.0);

    multiply(image_f, func_image3_f, m_image);
    multiply(blurred_image_f, compl_image3_f, m_bimage);

    Mat result_f;

    addWeighted(m_image, 1, m_bimage, 1, 0, result_f);

    result_f.convertTo(result, CV_8UC3);

    Mat result_hsv;
    Mat planes_hsv[3];
    Mat hue_saturated;

    cvtColor(result, result_hsv, CV_BGR2HSV);
    split(result_hsv, planes_hsv);
    planes_hsv[1].convertTo(hue_saturated, -1, 1, hue_gain);
    hue_saturated.copyTo(planes_hsv[1]);
    merge(planes_hsv, 3, result_hsv);
    
    cvtColor(result_hsv, result, CV_HSV2BGR);
    imshow("result", result);
}

void on_trackbar_start_focus(int, void*) {
    if (start_focus_slider > center_focus_slider) {
        setTrackbarPos("Start", "func_image", center_focus_slider);
        start_focus = (double) center_focus_slider;
    } else if (2*center_focus_slider - start_focus_slider > 100) {
        setTrackbarPos("Start", "func_image", 
                       2*center_focus_slider - 100);
        start_focus = 2*center_focus_slider - 100;
    } else {
        start_focus = (double) start_focus_slider;
    }
    start_focus = (double) start_focus_slider;
    composeResult();
}

void on_trackbar_decay_strength(int, void*) {
    decay_strength = (double) decay_strength_slider;
    composeResult();
}

void on_trackbar_center_focus(int, void*) {
    if ( center_focus_slider < start_focus_slider ) {
        setTrackbarPos("Center", "func_image", start_focus_slider);
        center_focus = (double) start_focus_slider;
    } else if (2*center_focus_slider - start_focus_slider > 100) { 
        setTrackbarPos("Center", "func_image",
                       (100 + start_focus_slider)/2);
        center_focus = (100 + start_focus_slider)/2;
    } else {    
        center_focus = (double) center_focus_slider;
    }
    composeResult();
}

void on_trackbar_hue_gain(int, void*) {
    hue_gain = hue_gain_slider;
    composeResult();
}

int main(int argc, char** argv) {
    if (argc != 2) {
        cout << "usage: " << argv[0] << " <img1>"
             << endl;
        exit(1);
    }

    image = imread(argv[1]);
    temp_image = image.clone();
    for (int i = 0; i < 100; ++i) {
        GaussianBlur(temp_image, blurred_image, Size(3, 3), 0, 0);
        temp_image = blurred_image.clone();
    }

    func_image = Mat(image.rows, image.cols, CV_8UC1, Scalar(255));
    compl_image = Mat(image.rows, image.cols, CV_8UC1, Scalar(255));

    namedWindow("func_image", WINDOW_NORMAL);
    namedWindow(    "result", WINDOW_NORMAL);

    sprintf( TrackbarName, "Start" );
    createTrackbar( TrackbarName, "func_image",
                    &start_focus_slider,
                    start_focus_slider_max,
                    on_trackbar_start_focus );
    on_trackbar_start_focus(start_focus_slider, 0 );


    sprintf( TrackbarName, "Decay" );
    createTrackbar( TrackbarName, "func_image",
                    &decay_strength_slider,
                    decay_strength_slider_max,
                    on_trackbar_decay_strength );
    on_trackbar_decay_strength(decay_strength_slider, 0);

    sprintf( TrackbarName, "Center" );
    createTrackbar( TrackbarName, "func_image",
                    &center_focus_slider,
                    center_focus_slider_max,
                    on_trackbar_center_focus );
    on_trackbar_center_focus(center_focus_slider, 0);

    sprintf( TrackbarName, "Hue Gain" );
    createTrackbar( TrackbarName, "func_image",
                    &hue_gain_slider,
                    hue_gain_slider_max,
                    on_trackbar_hue_gain );
    on_trackbar_hue_gain(hue_gain_slider, 0);


    waitKey(0);

    exit(0);
}
```
