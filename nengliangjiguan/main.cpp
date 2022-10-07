#include <iostream>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

void draw_rect(Mat &image)
{
	Mat channels[3];
	split(image, channels);
	Mat thresh;
	threshold(channels[0] - channels[2], thresh, 80, 255, THRESH_BINARY_INV);
	Mat element1 = getStructuringElement(MORPH_RECT, Size(5, 5));
	morphologyEx(thresh, thresh, MORPH_OPEN, element1);
	floodFill(thresh, Point(0, 0), Scalar(0));
	Mat element2 = getStructuringElement(MORPH_RECT, Size(25, 25));
	morphologyEx(thresh, thresh, MORPH_CLOSE, element2);
	vector<vector<Point>>contours;
	vector<Vec4i>hierarchy;
	findContours(thresh, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
	double area[20] = { 0 };
	Point edge;
	for (int i = 0; i < hierarchy.size(); i++)
	{
		area[i] = contourArea(contours[i]);
		if (area[i] < 800)
		{
			Rect rect = boundingRect(contours[i]);
			float radio = double(rect.width) / double(rect.height);
			if (radio > 0.9 || radio < 1.1)
			{
				Point2f point_array[4];
				RotatedRect box = minAreaRect(contours[i]);
				box.points(point_array);
				for (int j = 0; j < 4; j++)
				{
					line(image, point_array[j], point_array[(j + 1) % 4], Scalar(0, 0, 255), 2, 8);
				}
				edge = box.center;
			}
		}
	}
	Mat thresh2;
	threshold(channels[0] - channels[2], thresh2, 100, 255, THRESH_BINARY);
	Mat element = getStructuringElement(MORPH_RECT, Size(5, 5));
	dilate(thresh2, thresh2, element);
	vector<vector<Point>>contours2;
	findContours(thresh2, contours2, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	Point center;
	for (int i = 0; i < contours2.size(); i++)
	{
		double area2 = contourArea(contours2[i]);
		if (area2 < 400 && area2 > 200)
		{
			Rect rect = boundingRect(contours2[i]);
			float radio = double(rect.width) / double(rect.height);
			if (radio < 1.1 && radio > 0.9)
			{
				rectangle(image, rect, Scalar(0, 255, 255), 2);
			}
			center.x = rect.x + rect.width / 2;
			center.y = rect.y + rect.height / 2;
		}
	}
	
	const float fx = 1604;
	const float width_pic_img = 110;
	const float pix_dis = 480;
	float dx = width_pic_img / pix_dis;
	float f = fx * dx;
	const float width = 700;
	float width_pic =sqrt(pow(edge.x - center.x, 2) + pow(edge.y - center.y, 2));
	float width_picture = width_pic * dx;
	float distance = f * width / width_picture;
	cout << "distance:" << distance << "mm" << endl;

	if (edge.x - center.x != 0)
	{
		float kLine = (edge.y - center.y) / (edge.x - center.x);
		float tan_line = kLine / (1 + kLine);
		float line_arctan = atan(tan_line);
		float angle =  line_arctan * 180.0 / 3.1415926;
		putText(image, "angle:", Point(5, 20), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255), 1);
		string angle1 = to_string(angle);
		putText(image, angle1, Point(55, 20), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255), 1);
	}
	else
	{
		putText(image, "angle:90.000000", Point(5, 20), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255), 1);
	}
	
}

int main()
{
	VideoCapture capture;
	capture.open("nengliangjiguan.avi");
	if (!capture.isOpened())
	{
		cout << "can not find..." << endl;
		system("pause");
		return -1;
	}
	Mat image;
	while (capture.read(image))
	{
		draw_rect(image);


		imshow("image", image);
		namedWindow("image", 0);
		char key = waitKey(20);
		if (key == 27)
		{
			break;
		}
	}
	capture.release();
	destroyAllWindows();
	return 0;
}