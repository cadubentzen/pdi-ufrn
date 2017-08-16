#include <iostream>
#include <cstdlib>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

void removeBubblesInBoundaries(Mat& image) {
	Size imgSize = image.size();

	// First loop through first and last line
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
		if (image.at<uchar>(i, 0) == 255) {
			floodFill(image, Point(0, i), Scalar(0));
		}
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
				floodFill(image, 
						  Point(j, i), 
						  Scalar(rand()%127 + 128));
				num_bubbles++;
			}
		}
	}

	return num_bubbles;
}

int main(int argc, char** argv){
	Mat image;

  	image= imread("bolhas.png",CV_LOAD_IMAGE_GRAYSCALE);
  	if(!image.data) {
    	cout << "failed to open bolhas.png" << endl;
		exit(1);
  	}

  	namedWindow("Original", WINDOW_AUTOSIZE);
  	imshow("Original", image);

	removeBubblesInBoundaries(image);
	namedWindow("NoBoundaries", WINDOW_AUTOSIZE);
  	imshow("NoBoundaries", image);

	unsigned num_bubbles = countBubbles(image);
	cout << "Number of bubbles: " << num_bubbles << endl;
  	namedWindow("Labeled", WINDOW_AUTOSIZE);
  	imshow("Labeled", image);

  	waitKey();
  	
	return 0;
}
