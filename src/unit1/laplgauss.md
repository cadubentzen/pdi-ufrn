## Laplacian of Gaussian

Here is a comparation between the application of a Laplacian filter and the Laplacian of Gaussian. In order to obtain the Laplacian of Gaussian, the convolution of the two 3x3 kernels was calculated previously, which results in a 5x5 kernel. For this, a simple multiplication in Python with Scipy helped:

```python
import numpy as np
from scipy import signal

gauss = np.array([[1,2,1],[2,4,2],[1,2,1]])
laplacian = np.array([[0,-1,0],[-1,4,-1],[0,-1,0]])

signal.convolve2d(laplacian, gauss)

# Result:
# [[ 0, -1, -2, -1,  0],
#  [-1,  0,  2,  0, -1]
#  [-2,  2,  8,  2, -2],
#  [-1,  0,  2,  0, -1]
#  [ 0, -1, -2, -1,  0]]

```

![Laplacian of Gaussian](./results/result_laplgauss.gif)

It is possible to notice that the borders now have a higher intensity, although more noise seems to show up with the Laplacian of Gaussian. It seems that due to the webcam pepper and salt noise, the gaussian filter does not help to cut it off, but rather increase its intensity.

#### laplgauss.cpp
```c++
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

void printmask(Mat &m){
    for(int i=0; i<m.size().height; i++){
        for(int j=0; j<m.size().width; j++){
            cout << m.at<float>(i,j) << ",";
        }
        cout << endl;
    }
}

void menu(){
    cout << "\npress key to activate filter: " << endl
         << "a - absolute value" << endl
         << "m - average" << endl
         << "g - gauss" << endl
         << "v - vertical" << endl
         << "h - horizontal" << endl
         << "l - laplacian" << endl
         << "x - laplacian of gaussian" << endl
         << "esc - exit" << endl;
}

int main(int argvc, char** argv){
    VideoCapture video;
    float media[] = {1,1,1,
                     1,1,1,
                     1,1,1};
    
    float gauss[] = {1,2,1,
                     2,4,2,
                     1,2,1};
    
    float horizontal[]={-1,0,1,
                        -2,0,2,
                        -1,0,1};
    
    float vertical[]={-1,-2,-1,
                       0,0,0,
                       1,2,1};
    
    float laplacian[]={0,-1,0,
                      -1,4,-1,
                       0,-1,0};

    float laplacian_of_gaussian[] = {0,-1,-2,-1,0,
                                     -1,0,2,0,-1,
                                     -2,2,8,2,-2,
                                     -1,0,2,0,-1,
                                     0,-1,-2,-1,0};

    Mat cap, frame, frame32f, frameFiltered;
    Mat mask(3,3,CV_32F), mask1;
    Mat result, result1;
    double width, height, min, max;
    int absolut;
    char key;

    video.open(0); 
    if(!video.isOpened()) 
        return -1;

    width=video.get(CV_CAP_PROP_FRAME_WIDTH);
    height=video.get(CV_CAP_PROP_FRAME_HEIGHT);

    cout << "width=" << width << "\n";;
    cout << "height =" << height<< "\n";;

    namedWindow("original", WINDOW_NORMAL);
    namedWindow("spatialfilter", WINDOW_NORMAL);

    mask = Mat(3, 3, CV_32F, media); 
    scaleAdd(mask, 1/9.0, Mat::zeros(3,3,CV_32F), mask1);
    swap(mask, mask1);
    absolut=1; // calcs abs of the image

    menu();
    for(;;){
        video >> cap; 
        cvtColor(cap, frame, CV_BGR2GRAY);
        flip(frame, frame, 1);
        imshow("original", frame);
        frame.convertTo(frame32f, CV_32F);
        filter2D(frame32f, frameFiltered, frame32f.depth(), mask, Point(1,1), 0);
        if(absolut){
            frameFiltered=abs(frameFiltered);
        }
        frameFiltered.convertTo(result, CV_8U);
        imshow("spatialfilter", result);
        key = (char) waitKey(10);
        if( key == 27 ) break; // esc pressed!
        switch(key){
            case 'a':
                menu();
                absolut=!absolut;
                break;
            case 'm':
                menu();
                mask = Mat(3, 3, CV_32F, media);
                scaleAdd(mask, 1/9.0, Mat::zeros(3,3,CV_32F), mask1);
                mask = mask1;
                printmask(mask);
                break;
            case 'g':
                menu();
                mask = Mat(3, 3, CV_32F, gauss);
                scaleAdd(mask, 1/16.0, Mat::zeros(3,3,CV_32F), mask1);
                mask = mask1;
                printmask(mask);
                break;
            case 'h':
                menu();
                mask = Mat(3, 3, CV_32F, horizontal);
                printmask(mask);
                break;
            case 'v':
                menu();
                mask = Mat(3, 3, CV_32F, vertical);
                printmask(mask);
                break;
            case 'l':
                menu();
                mask = Mat(3, 3, CV_32F, laplacian);
                printmask(mask);
                break;
            case 'x':
                menu();
                mask = Mat(5, 5, CV_32F, laplacian_of_gaussian);
                printmask(mask);
            default:
                break;
        }
    }
    return 0;
}
```
