## Counting bubbles with holes

This example makes use of `floodFill`, which consists of painting an area that has the same color, replacing it by another color. Here this technique is used for counting and labeling bubbles in a black and white picture. Besides that, in this algorithm, bubbles with holes are counted too.

#### Original image
![Original](./results/result_bubbles_original.png)

#### No boundaries
![No boundaries](./results/result_bubbles_noboundaries.png)

#### Labeled
![Labeled](./results/result_bubbles_colored.png)

To run the program: `./bubbles <bubbles_img>`. An example is illustrated above with an example as input. First we remove the bubbles that are in the boundaries because we can't know for sure that they have holes or not (in case this picture was taken from real objects), then we paint the bubbles and bubbles with holes are paint as blue (in RGB: 0, 0, 255).

One important aspect of the algorith below is that it runs on a BGR image, not greyscale. This is done to overcome the problem of having a limit of 255 bubbles on counting if the same thing was done in greyscale. In this implementation I allowed from BGR = (0,0,1) up until BGR = (50,255,255) for labeling. This way it is possible to count up to 3276799 bubbles. The blue channel was limited to 50 for allowing bubbles with holes to be painted full blue BGR = (255,0,0) and be distinguished from the others.

#### bubbles.cpp
```c++
#include <iostream>
#include <cstdlib>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

inline void incrementColor(Vec3b &color) {
    if (color[2] < 255) {
        color[2]++;
    } else if (color[1] < 255) {
        color[1]++;
        color[2] = 0;
    } else if (color[0] < 50) {
        color[0]++;
        color[1] = 0;
        color[2] = 0;
    } else {
        cout << "Max of bubbles for this algorithm is 3276799." << endl;
        exit(1);
    }
}

int main(int argc, char** argv){
    if (argc != 2) {
        cout << "usage:" << argv[0] << " <bubbles_image>" << endl
             << "\t where <bubbles_image> should be a black "
             << "and white image of bubbles" << endl;
        exit(1);
    }

    Mat image;

    image = imread(argv[1]);
    if(!image.data) {
        cout << "failed to open bolhas.png" << endl;
        exit(1);
    }

    namedWindow("Original", WINDOW_AUTOSIZE);
    imshow("Original", image);

    Size imgSize = image.size();
    Vec3b white;
    white[0] = 255; white[1] = 255; white[2] = 255;

    // First remove the bubbles in boundaries

    // Loop through first and last line
    for(int i = 0; i < imgSize.width; ++i) {
        // First line
        if (image.at<Vec3b>(0, i) == white) {
            floodFill(image, Point(i, 0), Scalar(0, 0, 0));
        }
        // Last line
        if (image.at<Vec3b>(imgSize.height-1, i) == white) {
            floodFill(image, Point(i, imgSize.height-1), Scalar(0, 0, 0));
        }
    }

    // Loop through first and last column
    for(int i = 0; i < imgSize.height; ++i) {
        // First column
        if (image.at<Vec3b>(i, 0) == white) {
            floodFill(image, Point(0, i), Scalar(0, 0, 0));
        }
        // Last column
        if (image.at<Vec3b>(i, imgSize.width-1) == white) {
            floodFill(image, Point(imgSize.width-1, i), Scalar(0, 0, 0));
        }
    }

    namedWindow("noBoundaries", WINDOW_AUTOSIZE);
    imshow("noBoundaries", image);  
    
    // Then first count bubbles with holes or not
    unsigned num_bubbles = 0;
    // Initial count color is [0,0,1]
    Vec3b color;
    color[0] = 0; color[1] = 0; color[2] = 1;

    for (int i = 0; i < imgSize.height; ++i) {
        for (int j = 0; j < imgSize.width; ++j) {
            if (image.at<Vec3b>(i, j) == white) {
                floodFill(image, Point(j, i), 
                          Scalar(color[0], color[1], color[2]));
                num_bubbles++;
                incrementColor(color);
            }
        }
    }

    cout << "Number of bubbles = " << num_bubbles << endl;

    // Now count bubbles with holes
    
    unsigned num_bubbles_with_holes = 0;
    Mat image_aux = image.clone();

    color[0] = 0; color[1] = 0; color[2] = 1;

    for (int i = 0; i < num_bubbles; ++i) {
        unsigned num_pixels_bg_before = 
            floodFill(image_aux, Point(0, 0), Scalar(0, 0, 0));
        
        Point first_pixel_with_color (0, 0);

        for (int i2 = 0; i2 < imgSize.height; ++i2) {
            for (int j = 0; j < imgSize.width; ++j) {
                if (image_aux.at<Vec3b>(i2, j) == color) {
                    first_pixel_with_color = Point(j, i2);
                    break;
                }
            }
        }
        if (first_pixel_with_color.x == 0 &&
            first_pixel_with_color.y == 0) {
            cerr << "No pixel with this color found. Review algorithm." << endl;
            exit(1);
        }

        unsigned num_pixels_with_color = 
            floodFill(image_aux, first_pixel_with_color, 
                                 Scalar(0, 0, 0));

        unsigned num_pixels_bg_after = 
            floodFill(image_aux, Point(0, 0), Scalar(0, 0, 0));

        if (num_pixels_bg_after > num_pixels_bg_before + num_pixels_with_color) {
            num_bubbles_with_holes++;
            floodFill(image, first_pixel_with_color,
                      Scalar(255, 0, 0));
        } else {
            floodFill(image, first_pixel_with_color,
                      Scalar(rand()%51, rand()%256, rand()%256));
        }

        incrementColor(color);
    }

    namedWindow("colored", WINDOW_AUTOSIZE);
    imshow("colored", image);

    cout << "Number of bubbles with holes = " << num_bubbles_with_holes << endl;

    waitKey();
    
    return 0;
}
```
