## Pointillism using Canny filter

This exercise consists of using Canny filter to produce pontillism images. One could implement a simple pointillism algorithm with circles that have the same size. However, a more interesting painting could be to use Canny filters with varying thresholds in order to draw radius with multiple radius.

The following result was produced using the algorithm as mentioned above. For every round, the minimum threshold is increased and the circle radius decreases. Also, some randomness helps as well: 

- Some jitter is added to every circle painted, so the circles do not look perfectly aligned (simulating how a human would draw)
- The circles are not drawn in progressive order. Instead, every iteration the drawing order is shuffled so there is not perfect superposition in any direction.

![Result of pointillism with Canny](./results/result_pointillism_canny.png)

#### pointillism_canny.cpp
```c++
#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <numeric>
#include <ctime>
#include <cstdlib>

using namespace std;
using namespace cv;

#define JITTER 15
#define ROUNDS 3
#define RADIUS 30

vector<int> yrange;
vector<int> xrange;

int width, height;

void init_ranges(Mat &image, int step) {
    
    cout << __func__ << endl;
    
    width  = image.size().width;
    height = image.size().height;

    xrange.resize(height/step);
    yrange.resize(width/step);

    iota(xrange.begin(), xrange.end(), 0); 
    iota(yrange.begin(), yrange.end(), 0);

    for (auto &i : xrange) {
        i = (i * step) + (step / 2);
    }

    for (auto &j : yrange) {
        j = (j * step) + (step / 2);
    }
}

void initial_round(Mat &image, Mat &points, Mat &image_color) {
    
    cout << __func__ << endl;
    
    random_shuffle(xrange.begin(), xrange.end());
    
    for(auto i : xrange) {
        random_shuffle(yrange.begin(), yrange.end());
        for(auto j : yrange) {
            int x = i+rand()%(2*JITTER)-JITTER+1;
            int y = j+rand()%(2*JITTER)-JITTER+1;
            Vec3b color = image_color.at<Vec3b>(x,y);
            circle(points,
                   cv::Point(y,x),
                   RADIUS,
                   Scalar(color),
                   -1,
                   CV_AA);
        }
    }
}

void draw_circles(Mat &image, Mat &points, Mat &image_color, 
                  int thresh, int radius) {
    Mat border;

    cout << __func__ << endl;

    Canny(image, border, thresh, 3*thresh);

    random_shuffle(xrange.begin(), xrange.end());
    
    for(auto i : xrange) {
        random_shuffle(yrange.begin(), yrange.end());
        for(auto j : yrange) {
            int border_point = border.at<uchar>(i,j);
            if (border_point == 255) {
                int x = i+rand()%(2*JITTER)-JITTER+1;
                int y = j+rand()%(2*JITTER)-JITTER+1;
                Vec3b color = image_color.at<Vec3b>(x,y);
                circle(points,
                       cv::Point(y,x),
                       radius,
                       Scalar(color),
                       -1,
                       CV_AA);
            }
        }
    }
} 

int main(int argc, char** argv){
    if (argc != 2) {
        cout << "usage: " << argv[0] << " image.png" << endl;
        exit(1);
    }

    Mat image, image_color, points;

    image_color = imread(argv[1]);

    cvtColor(image_color, image, CV_BGR2GRAY);

    namedWindow("pontos", WINDOW_NORMAL);

    srand(time(0));

    if(!image.data){
        cout << "Could not open" << argv[1] << endl;
        exit(1);
    }

    init_ranges(image, 5);

    points = Mat(height, width, CV_8UC3, 
                 Scalar(255, 255, 255));

    initial_round(image, points, image_color);

    for (int i = 0, thresh = 20; 
         i < ROUNDS;
         ++i, thresh += (100 - 20)/(ROUNDS-1) ) {

        init_ranges(image, ROUNDS-i); 
        draw_circles(image, points, image_color, 
                     thresh, 5*(ROUNDS-i));
    }
    
    imshow("pontos", points);

    while(1)
        if(waitKey(10) == 27) break;

    return 0;
}
```
