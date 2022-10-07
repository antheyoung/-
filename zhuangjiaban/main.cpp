#include <iostream>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;
const int kThreashold = 150;
const int kMaxVal = 255;

void draw_rect(Mat &image)
{
	Mat gray;
	cvtColor(image, gray, COLOR_BGR2GRAY);
	Mat gaussian;
	GaussianBlur(gray, gaussian, Size(3, 3), 0);
	Mat thresh;
	threshold(gray, thresh, kThreashold, kMaxVal, 0);
	vector<vector<Point>>contours;
	findContours(thresh, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	Rect point_array[2];
	int j = 0;
	for (int i = 0; i < contours.size(); i++)
	{
		double area = contourArea(contours[i]);
		if (area > 200)
		{
			Rect rect = boundingRect(contours[i]);
			float radio = double(rect.width) / double(rect.height);
			if (radio > 2.5 || radio < 0.5)
			{
				point_array[j] = rect;
				j++;
			}
		}
	}
	Point point1 = Point(point_array[0].x + point_array[0].width / 2, point_array[0].y);
	Point point2 = Point(point_array[0].x + point_array[0].width / 2, point_array[0].y + point_array[0].height);
	Point point3 = Point(point_array[1].x + point_array[1].width / 2, point_array[1].y);
	Point point4 = Point(point_array[1].x + point_array[1].width / 2, point_array[1].y + point_array[1].height);
	Point point0 = Point((point1.x + point4.x) / 2, (point1.y + point4.y) / 2);
	Point points[4] = { point1,point2,point3,point4 };
	line(image, points[0], points[3], Scalar(255, 0, 0), 1);
	line(image, points[1], points[2], Scalar(255, 0, 0), 1);
	circle(image, point0, 3, Scalar(0, 0, 255), -1);

	const float fx = 1604;
	const float width_pic_img = 160;
	const float pix_dis = 1280;
	float dx = width_pic_img / pix_dis;
	float f = fx * dx;
	const float width = 230;
	float width_pic = points[3].x - points[0].x;
	if (width_pic < 0) width_pic = -width_pic;
	float width_picture = width_pic * dx;
	float distance = f * width / width_picture;
	cout << "distance:" << distance <<"mm"<<endl;
}


int main()
{
	VideoCapture capture;
	VideoWriter writer;
	capture.open("zhuangjiaban.avi");
	double fps = 30;
	string result_name = "zhuangjiabanXiaoGuo.avi";
	writer.open(result_name,capture.get(CAP_PROP_FOURCC),fps,Size(capture.get(CAP_PROP_FRAME_WIDTH),capture.get(CAP_PROP_FRAME_HEIGHT)),true);
	if(!capture.isOpened())
	{
		cout << "can not find..." << endl;
		system("pause");
		return -1;
	}
	Mat image;
	while (capture.read(image))
	{
		draw_rect(image);

		imshow("image",image);
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