## Swapping regions

This example consists of swapping the four regions of a picture, given a order passed by command line. For example, running `./swap_regions lenna.png 4 3 2 1` swaps the regions as follows, since the original sequence is `1 2 3 4` (left to right, top to bottom)

![Lenna swapped](./results/result_swap.png "Lenna swapped")

#### swap\_regions.cpp
```c++
#include <iostream>
#include <cstdlib>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

#define CHECK_VALID_RANGE(x) if (x < 0 || x > 3) \
    {cout << "Invalid region number in sequence" << endl; exit(1);}

int main(int argc, char** argv){
    if (argc != 6) {
        cout << "usage: " << endl
             << "\t" << argv[0] << " <img_path> <sequence with 4 numbers from 1 to 4" 
             << " separated by spaces>" << endl
             << "\tExample: " << argv[0] << " ../img/lenna.png 4 3 2 1" << endl
             << "\tThe sequence 1 2 3 4 is the original image, swapping these numbers" 
             << " swaps the four regions of the image." << endl;

        exit(1);
    }
    
    Mat image = imread(argv[1], CV_LOAD_IMAGE_COLOR);
    
    if (!image.data)
        cout << "Could not open " << argv[1] << endl;

    int sequence[4];
    for (int i = 0; i < 4; ++i) {
        sequence[i] = atoi(argv[i+2]) - 1;
        CHECK_VALID_RANGE(sequence[i]);
    }

    int rows = image.rows;
    int cols = image.cols;

    Rect rois[4] = {
        Rect(     0,      0, rows/2, cols/2),
        Rect(rows/2,      0, rows/2, cols/2),
        Rect(     0, cols/2, rows/2, cols/2),
        Rect(rows/2, cols/2, rows/2, cols/2)
    };

    namedWindow(argv[1],WINDOW_AUTOSIZE);

    Mat3b swapped (rows, cols, Vec3b(0, 0, 0));

    for (int i = 0; i < 4; ++i) {
        Mat region (image, rois[ sequence[i] ]);
        region.copyTo(swapped(rois[i]));
    }

    imshow(argv[1], swapped);  
    waitKey();

    exit(0);
}
```
