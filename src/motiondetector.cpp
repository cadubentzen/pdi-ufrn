#include <iostream>
#include <cstdlib>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main(int argc, char** argv){
	if (argc != 2) {
		cout << "usage: " << argv[0] << " <thresh_motion>" << endl
			 << "Where thresh_motion is a threshold for "
			 << "relative difference of histograms between 0 and 100." << endl;
		exit(1);
	}

	float thresh_motion = atof(argv[1]);

	Mat image, grey;
	int width, height;
	VideoCapture cap;

	int histSize = 256;
	float range[] = {0, 256};
	const float* histRange = {range};
	Mat hist;

	float avg = -1, prev_avg = -1;

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

		calcHist(&grey, 1, 0, Mat(), hist, 1, &histSize, &histRange);

		avg = 0;
		for (int i = 0; i < 256; ++i) {
			avg += i * hist.at<float>(i) / ( (float)(width*height) );
		}

		if (prev_avg > 0 && // First iteration and not zero for division
			abs(avg-prev_avg)/prev_avg > thresh_motion/100.0 ) {

			cout << "Detected movement!!" << endl;
		}

		prev_avg = avg;
	
		imshow("grey", grey);
		
		if(waitKey(1000) >= 0) break;
	}
	exit(0);
}
