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
