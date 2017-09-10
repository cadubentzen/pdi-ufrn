## Digital Image Processing

This website is a showcase of small projects developed during the course of Digital Image Processing offered by Universidade Federal do Rio Grande do Norte, Department of Computer Engineering.

These exercises are based on the OpenCV tutorial provided by professor Agostinho Brito Jr (http://agostinhobritojr.github.io/tutoriais/pdi/). 

OpenCV 2.4 has been used in this material.

### Makefile

```make
CXX = g++
CXXFLAGS = `pkg-config --cflags --libs opencv` -g

SOURCES = regions.cpp \
		  swap_regions.cpp \
		  equalize.cpp \
		  motiondetector.cpp \
		  bubbles.cpp

OUTPUTS = $(basename $(SOURCES))

all: $(OUTPUTS)

%: %.cpp
	$(CXX) $< -o bin/$@ $(CXXFLAGS)

clean:
	-rm -rf bin/*

```

### Negative of a region

```c++
#include <iostream>
#include <cstdlib>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

#define CHECK_VALID_RANGE(x, dimension) if(x < 0 || x >= dimension) \
	{cout << "Invalid value for "#x << endl; exit(1);}

void checkValidRange(int x, int dimension) {
	if (x < 0 || x >= dimension) exit(1);
}

int main(int argc, char** argv){
	if (argc != 2 && argc != 6) {
		cout << "usage: " << endl
			 << "\t" << argv[0] << "<img_path>" 
			 << " // To get image dimensions" << endl
			 << "\t" << argv[0] << "<img_path> <x1> <y1> <x2> <y2>" 
			 << " // To negative the rectangular area" << endl;

		exit(1);
	}
	
	Mat image = imread(argv[1], CV_LOAD_IMAGE_COLOR);
	
	if (!image.data)
		cout << "Could not open " << argv[1] << endl;

	int cols = image.cols;
	int rows = image.rows;

	cout << "###### Image dimensions ######" << endl
		 << "Columns: "  << cols << endl
		 << "Rows: "     << rows << endl
		 << "##############################" << endl;

	if (argc == 2) exit(0);

	int x1 = atoi(argv[2]); CHECK_VALID_RANGE(x1, rows);
	int y1 = atoi(argv[3]); CHECK_VALID_RANGE(y1, cols);
	int x2 = atoi(argv[4]); CHECK_VALID_RANGE(x2, rows);
	int y2 = atoi(argv[5]); CHECK_VALID_RANGE(y2, cols);

	namedWindow(argv[1],WINDOW_AUTOSIZE);

	for (int i = min(x1, x2); i < max(x1, x2); ++i) {
		for (int j = min(y1, y2); j < max(y1, y2); ++j) {
			image.at<Vec3b>(i,j)[0] = 255- image.at<Vec3b>(i,j)[0];
			image.at<Vec3b>(i,j)[1] = 255- image.at<Vec3b>(i,j)[1];
			image.at<Vec3b>(i,j)[2] = 255- image.at<Vec3b>(i,j)[2];
		}
	}

	imshow(argv[1], image);  
	waitKey();

	exit(0);
}

```

### Swapping regions

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
		Rect(         0,          0, rows/2, cols/2),
		Rect(    rows/2,          0, rows/2, cols/2),
		Rect(         0,     cols/2, rows/2, cols/2),
		Rect(    rows/2,     cols/2, rows/2, cols/2)
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

### Counting bubbles with holes

```c++
#include <iostream>
#include <cstdlib>
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

		unsigned num_pixels_with_color = 
			(unsigned) floodFill(image, first_pixel_with_color, Scalar(0));

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

  	image = imread("../img/bolhas.png",CV_LOAD_IMAGE_GRAYSCALE);
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

```

### Histogram equalization

```
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

```

### Motion detection

```
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

```

