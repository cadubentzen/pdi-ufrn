#include <iostream>
#include <cmath>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

double start_focus    = 20;
double decay_strength = 50;
double center_focus   = 50;
int    hue_gain       = 20;

Mat image, temp_image, blurred_image;
Mat func_image, compl_image;
Mat m_image, m_bimage;
Mat result;

int num_frame = 1;

void drawFuncImage() {
	uchar pixel_value;
	double x;
	double den = (decay_strength > 0 ? decay_strength/10 : 0.1);
	double func_val;
	for (int i = 0; i < func_image.rows; ++i) {
		x = (double) i*100.0 /func_image.rows;
		func_val =
		   	( tanh( (x-start_focus)/den ) - 
			  tanh ( ( x-(2*center_focus-start_focus) )/den ) ) / 2;
		pixel_value = (uchar) (255*func_val);
		for (int j = 0; j < func_image.cols; ++j) {
			func_image.at<uchar>(i,j) = pixel_value;
			compl_image.at<uchar>(i,j) = 255 - pixel_value;
		}
	}
}

void composeResult() {
	drawFuncImage();
	
	Mat image_f, blurred_image_f;
	image.convertTo(image_f, CV_32F);
	blurred_image.convertTo(blurred_image_f, CV_32F);

	Mat func_image3, compl_image3;
	
	Mat t_func[]  = { func_image,  func_image,  func_image};
	Mat t_compl[] = {compl_image, compl_image, compl_image};

	merge( t_func, 3,  func_image3);
	merge(t_compl, 3, compl_image3);

	Mat func_image3_f, compl_image3_f;
	func_image3.convertTo(func_image3_f, CV_32F, 1.0/255.0);
	compl_image3.convertTo(compl_image3_f, CV_32F, 1.0/255.0);

	multiply(image_f, func_image3_f, m_image);
	multiply(blurred_image_f, compl_image3_f, m_bimage);

	Mat result_f;

	addWeighted(m_image, 1, m_bimage, 1, 0, result_f);
	result_f.convertTo(result, CV_8UC3);
	
	Mat result_hsv;
	Mat planes_hsv[3];
	Mat hue_saturated;

	cvtColor(result, result_hsv, CV_BGR2HSV);
	split(result_hsv, planes_hsv);
	planes_hsv[1].convertTo(hue_saturated, -1, 1, hue_gain);
	hue_saturated.copyTo(planes_hsv[1]);
	merge(planes_hsv, 3, result_hsv);
	
	cvtColor(result_hsv, result, CV_HSV2BGR);
}


int main(int argc, char** argv) {
	if (argc != 8) {
		cout << "usage: " << argv[0] << " <video_input> "
			 << "<video_output> "
			 << "<start_focus> <decay> <center_focus> " 
			 << "<hue_gain> <num_frame>" << endl << endl
			 << "\tWhere start_focus, decay and center may be "
			 << "between 0 and 100;" << endl
			 << "<hue_gain> goes between 0 and 255;" << endl
			 << "\t<num_frame> is the number of frames in the original to "
			 << "the created. (stop motion effect)"
			 << "\tAnd the output video must have an extension .avi"
			 << endl;
		exit(1);
	}

	VideoCapture cap (argv[1]);

	if (!cap.isOpened()){
		cout << "Failed to open input file " << argv[1] << endl;
		exit(1);
	}

	num_frame = atoi(argv[7]);

	VideoWriter wri (argv[2], CV_FOURCC('D','I','V','X'), 
					 cap.get(CV_CAP_PROP_FPS)/num_frame,
					 Size(cap.get(CV_CAP_PROP_FRAME_WIDTH), 
						  cap.get(CV_CAP_PROP_FRAME_HEIGHT)));
	
	if (!wri.isOpened()){
		cout << "Failed to open output file " << argv[2] << endl;
		exit(1);
	}


	start_focus    = atof(argv[3]);
	decay_strength = atof(argv[4]);
	center_focus   = atof(argv[5]);
	hue_gain       = atoi(argv[6]);

	cap >> image;
	if(image.empty()) exit(0);

	func_image  = Mat(image.rows, image.cols, CV_8UC1, Scalar(255));
	compl_image = Mat(image.rows, image.cols, CV_8UC1, Scalar(255));

	while(1) {
		
		for(int i = 0; i < num_frame; ++i) {
			cap >> image;
			if(image.empty()) exit(0);
		}

		temp_image = image.clone();
		for (int i = 0; i < 100; ++i) {
			GaussianBlur(temp_image, blurred_image, Size(3, 3), 0, 0);
			temp_image = blurred_image.clone();
		}

		composeResult();

		wri << result;
	}

	exit(0);
}
