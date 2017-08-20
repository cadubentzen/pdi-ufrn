#include <iostream>
#include <cstdlib>
#include <vector>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

void removeBubblesInBoundaries(Mat& image) {
	Size imgSize = image.size();

	// Loop through first and last line
	for(int i = 0; i < imgSize.width; ++i) {
		// First line
		if (image.at<uchar>(0, i) == 255) {
			floodFill(image, Point(i, 0), Scalar(0));
		}
		// Last line
		if (image.at<uchar>(imgSize.height-1, i) == 255) {
			floodFill(image, Point(i, imgSize.height-1), Scalar(0));
		}
	}

	// Loop through first and last column
	for(int i = 0; i < imgSize.height; ++i) {
		// First column
		if (image.at<uchar>(i, 0) == 255) {
			floodFill(image, Point(0, i), Scalar(0));
		}
		// Last column
		if (image.at<uchar>(i, imgSize.width-1) == 255) {
			floodFill(image, Point(imgSize.width-1, i), Scalar(0));
		}
	}	
}

unsigned countBubbles(Mat& image) {
	Size imgSize = image.size();
	unsigned num_bubbles = 0;

	for (int i = 0; i < imgSize.height; ++i) {
		for (int j = 0; j < imgSize.width; ++j) {
			if (image.at<uchar>(i, j) == 255) {
				floodFill(image, Point(j, i), Scalar(++num_bubbles));
				if (num_bubbles > 255) {
					cerr << "Maximum of bubbles is 255 for this algorithm" << endl; 
					exit(1);
				}
			}
		}
	}

	return num_bubbles;
}

unsigned countBackgroundPixels(Mat &image) {
	return (unsigned) floodFill(image, Point(0, 0), Scalar(0));
}

Point firstPixelWithColor(Mat& image, uchar color) {
	Size imgSize = image.size();
	for (int i = 0; i < imgSize.height; ++i) {
		for (int j = 0; j < imgSize.width; ++j) {
			if (image.at<uchar>(i, j) == color) return Point(j, i);
		}
	}
	cerr << "No pixel with this color found!" << endl;
	exit(1);
}

unsigned countBubblesWithHoles(Mat& imagesrc) {
	unsigned num_bubbles = countBubbles(imagesrc);
	unsigned num_bubbles_with_holes = 0;
	Mat image = imagesrc.clone();

	cout << "Number of bubbles: " << num_bubbles << endl;

	for (int i = 0; i < num_bubbles; ++i) {
		unsigned num_pixels_bg_before = countBackgroundPixels(image);
		Point first_pixel_with_color = firstPixelWithColor(image, i+1);

		unsigned num_pixels_with_color = (unsigned) floodFill(image, first_pixel_with_color, Scalar(0));

		unsigned num_pixels_bg_after = countBackgroundPixels(image);

		if (num_pixels_bg_after > num_pixels_bg_before + num_pixels_with_color) 
			num_bubbles_with_holes++;
	}

	return num_bubbles_with_holes;
}

void showColoredBubbles(Mat& image) {
	Mat colored_image;
	Size imgSize = image.size();
	cvtColor(image, colored_image, CV_GRAY2BGR);

	for (int i = 0; i < imgSize.height; ++i) {
		for (int j = 0; j < imgSize.width; ++j) {
			if (image.at<uchar>(i, j) != 0) {
				floodFill(colored_image, Point(j, i), 
					      Scalar(rand()%256, rand()%256, rand()%256) );
			}
		}
	}

	namedWindow("colored");
	imshow("colored", colored_image);
}

int main(int argc, char** argv){
	Mat image;

  	image = imread("bolhas.png",CV_LOAD_IMAGE_GRAYSCALE);
  	if(!image.data) {
    	cout << "failed to open bolhas.png" << endl;
		exit(1);
  	}

  	namedWindow("Original", WINDOW_AUTOSIZE);
  	imshow("Original", image);

	removeBubblesInBoundaries(image);

	unsigned num_bubbles_with_holes = countBubblesWithHoles(image);
	cout << "Number of bubbles with holes: " << num_bubbles_with_holes << endl;

	showColoredBubbles(image);

  	waitKey();
  	
	return 0;
}
