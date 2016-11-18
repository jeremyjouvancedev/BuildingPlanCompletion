#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

using namespace cv;
using namespace std;

Mat src; Mat src_gray, tmpSrc;
int thresh = 100;
int max_thresh = 255;
RNG rng(12345);
double ratio;
char *secondWind;
int x_ScreenSize;
int y_ScreenSize;

vector<Point> AreaContour;
double areaValue = 0;

/// Function header
void thresh_callback(int, void*);
void MouseCallBackFunc(int event, int x, int y, int flags, void* userdata);
void DrawWall(int, int, int);
void PutLonger();

/** @function main */
int main(int argc, char** argv)
{
	/// Load source image and convert it to gray
	src = imread(argv[1], 1);

	if (src.empty())
	{
		cout << "vide" << endl;
	}

	x_ScreenSize = GetSystemMetrics(SM_CXSCREEN);
	y_ScreenSize = GetSystemMetrics(SM_CYSCREEN) - 150;

	double ratioResize;
	ratioResize = y_ScreenSize/ (double)src.rows;

	double x_size(src.cols * ratioResize), y_size(src.rows * ratioResize);
	resize(src, src, Size(x_size, y_size));
	ratio = 114, 8648648648649;
	
	/// Convert image to gray and blur it
	cvtColor(src, src_gray, CV_BGR2GRAY);
	blur(src_gray, src_gray, Size(3, 3));

	/// Create Window
	char* source_window = "Source";
	secondWind = "SecondWind";
	namedWindow(secondWind, CV_WINDOW_AUTOSIZE);
	namedWindow(source_window, CV_WINDOW_AUTOSIZE);
	imshow(source_window, src);

	setMouseCallback("SecondWind", MouseCallBackFunc, NULL);
	createTrackbar(" Canny thresh:", "Source", &thresh, max_thresh, thresh_callback);
	thresh_callback(0, 0);
	
	waitKey(0);
	return(0);
}

void MouseCallBackFunc(int event, int x, int y, int flags, void* userdata)
{
	if (event == EVENT_LBUTTONDOWN)
	{
		AreaContour.push_back(Point2f(x, y));
		tmpSrc = src;
		DrawWall(x, y, 100);
		PutLonger();
		imshow(secondWind, tmpSrc);
		cout << "Nombre de points : " << AreaContour.size() << endl;
	}
	else if (event == EVENT_RBUTTONDOWN)
	{
		tmpSrc = src;
		DrawWall(x, y, 100);
		PutLonger();
		imshow(secondWind, tmpSrc);
		//cout << "Right button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
	}
	else if (event == EVENT_MBUTTONDOWN)
	{
		imwrite("result.png", tmpSrc);
		//cout << "Middle button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
	}
	else if (event == EVENT_MOUSEMOVE)
	{					
		//cout << "Mouse move over the window - position (" << x << ", " << y << ")" << endl;
	}
}


/** @function thresh_callback */

bool verifPoint(vector<Point> middle, Point middleAppend)
{
	for (int i = 0; i < middle.size(); i++)
	{
		if (middle[i].x >= middleAppend.x - 1 &&
			middle[i].x <= middleAppend.x + 1 && 
			middle[i].y >= middleAppend.y - 1 &&
			middle[i].y <= middleAppend.y + 1)
			return false;
	}
	return true;
}

void DrawWall(int x, int y, int r)
{
	Mat canny_output;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	vector<Point> tabMidle;
	vector<double> tabLVector;

	/// Detect edges using canny
	Canny(src_gray, canny_output, thresh, thresh * 2, 3);
	/// Find contours
	findContours(canny_output, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	/// Draw contours
	Mat drawing = Mat::zeros(canny_output.size(), CV_8UC3);
	for (int i = 0; i < contours.size(); i++)
	{
		Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		for (int w = 0; w < contours[i].size() - 1; w++)
		{
			Point x1(contours[i][w]), x2(contours[i][w + 1]);
			double lVector = sqrt(pow((x2.x - x1.x), 2) + pow((x2.y - x1.y), 2));
			lVector /= ratio;
			Point middle(x1.x + ((x2.x - x1.x) / 2),
				x1.y + (x2.y - x1.y) / 2);

			if (verifPoint(tabMidle, middle))
			{
				tabMidle.push_back(middle);
				tabLVector.push_back(lVector);
			}
		}
		//drawContours(drawing, contours, i, color, 2, CV_AA, hierarchy, 0, Point());
	}
	for (int i = 0; i < tabMidle.size(); i++)
	{	
		std::ostringstream strs;
		strs << tabLVector[i];
		string LongueurToStr = strs.str();		
		if (tabLVector[i] >= 0.5)
			putText(tmpSrc, LongueurToStr, tabMidle[i], FONT_HERSHEY_COMPLEX_SMALL, 1, Scalar(rand() % 255, rand() % 255, rand() % 255), 1, CV_AA);
	}
}

void PutLonger()
{
	Point AreaTextMiddle;
	double AreaValue(0);
	vector<Vec4i> hierarchy;
	vector<vector<Point> > contours;
	
	Mat vide = Mat::zeros(Size(x_ScreenSize, y_ScreenSize), CV_8UC1);
	if (AreaContour.size() >= 3)
	{
		for (int z = 0; z < AreaContour.size(); z++)
		{
			line(vide, AreaContour[z], AreaContour[(z + 1) % AreaContour.size()], Scalar(255), 1, CV_AA);
			AreaTextMiddle.x += AreaContour[z].x;
			AreaTextMiddle.y += AreaContour[z].y;
		}
		findContours(vide, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
		for (int w = 0; w < contours.size(); w++)
		{
			drawContours(tmpSrc, contours, w, Scalar(255, 0, 0), CV_FILLED, CV_AA, hierarchy, 0, Point());
			AreaValue += contourArea(contours[w]);
		}
		std::ostringstream strs;
		strs << AreaValue;
		string Value = strs.str();
		AreaTextMiddle.x /= AreaContour.size();
		AreaTextMiddle.y /= AreaContour.size();
		putText(tmpSrc, Value, AreaTextMiddle, FONT_HERSHEY_COMPLEX_SMALL, 1, Scalar(255, 255, 255), 1, CV_AA);	
	}
}

void thresh_callback(int, void*)
{
	/*Mat result;
	result = DrawWall();
	result = PutLonger(result);*/
	// Show in a window
	//imshow(secondWind, result);
}