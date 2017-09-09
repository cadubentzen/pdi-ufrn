#include <iostream>
#include <cstdlib>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main(int argc, char** argv){
	Mat image, grey;
	int width, height;
	VideoCapture cap;

	int histSize = 256;
	float range[] = {0, 256};
	const float* histRange = {range};
	Mat hist;

	cap.open(0);

	if (!cap.isOpened()){
		cout << "No cameras available" << endl;
		exit(1);
	}

	width  = cap.get(CV_CAP_PROP_FRAME_WIDTH);
	height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
	
	cout << "###### Image dimensions ######" << endl	
		 << "Width = " << width << endl
		 << "Height  = " << height << endl
		 << "##############################" << endl;

	while (1) {
		cap >> image;
		if(image.empty()) exit(1);

		cvtColor(image, grey, CV_BGR2GRAY);

		imshow("grey", grey);

		calcHist(&grey, 1, 0, Mat(), hist, 1, &histSize, &histRange);
		
		// Calculate accumulated histogram
		for (int i = 1; i < hist.rows; ++i) {
			hist.at<float>(i) += hist.at<float>(i-1);
		}
		
		// Normalize the accumulated histogram
		for (int i = 0; i < hist.rows; ++i) {
			hist.at<float>(i) *= 255/((float)(width*height));
		}

		// Repaint the new equalized image
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				grey.at<uchar>(i, j) = (uchar) hist.at<float>(grey.at<uchar>(i, j));
			}
		}

		imshow("equalized", grey);
	
		if(waitKey(30) >= 0) break;
	}
	exit(0);
}
