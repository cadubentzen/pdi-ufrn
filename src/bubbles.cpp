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
