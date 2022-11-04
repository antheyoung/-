#include <iostream>
#include <opencv2/opencv.hpp>
#include "armor_plate.h"
#include "light_bar.h"
#include "KalmanFilterX.hpp"

using namespace std;
using namespace cv;

int main()
{
	VideoCapture capture;
	VideoWriter writer;
	capture.open("../armor_plate.avi");
	writer.open("识别结果.avi",capture.get(CAP_PROP_FOURCC),30,Size(capture.get(CAP_PROP_FRAME_WIDTH),capture.get(CAP_PROP_FRAME_HEIGHT)),true);
	if (!capture.isOpened())
	{
		cout << "can not find..." << endl;
		return -1;
	}
	Mat src;
	Point my_points[5];
	uint8_t i = 0;
	Matx21f predicts_x;
	Matx21f predicts_y;
	armor_plate my_armor;
	light_bar my_light;
	int16_t s_x_last = 0;
	int16_t s_y_last = 0;
	while (capture.read(src))
	{

		// 获取前一帧图像装甲板中心位置
		if (i > 0)
		{
			s_x_last = my_points[4].x;
			s_y_last = my_points[4].y;
		}
		i++;
		// 获取当前帧图像装甲板中心位置
		vector<Rect> my_light_bars = {};
		my_light.find_light(src, my_light_bars);
		my_armor.get_bar(my_light_bars);
		my_armor.is_armor();
		my_armor.draw_rect(src);
		my_armor.give_point(my_points);
		int16_t s_x = my_points[4].x;
		int16_t s_y = my_points[4].y;
		// 计算速度
		if (i == 1)
			continue;
		float v_x = 0;
		float v_y = 0;
		int16_t dx = s_x - s_x_last;
		int16_t dy = s_y - s_y_last;
		float dt = 0.04;
		v_x = dx / dt;
		v_y = dy / dt;

		// 使用卡尔曼滤波对x坐标进行预测
		KalmanFilterX<2, 2> rect_x(10, 1e-10);
		// 设置X
		if (i == 2)
		{
			float Xmin_array[2] = {(float)s_x_last, v_x}; // 第二帧初始化 X
			Matx21f Xmin_Matx(Xmin_array);
			rect_x.init(Xmin_Matx, 1e-10);
		}
		else
		{
			rect_x.init(predicts_x, 1e-10); // 第三帧后初始化 X
		}

		// 初始化状态转移矩阵
		float Ax_array[4] = {1, dt, 0, 1};
		Matx22f Ax_Matx(Ax_array);
		rect_x.setA(Ax_Matx);
		// 初始化测量矩阵
		float Hx_array[4] = {1, 0, 0, 1};
		Matx22f Hx_Matx(Hx_array);
		rect_x.setH(Hx_Matx);
		// 给出测量值，再此处为图形直接识别值
		float measurex_array[2] = {(float)my_points[4].x, v_x};
		Matx21f measurex_Matx(measurex_array);
		// 进行数据融合，给出最优估计值
		rect_x.predict();
		predicts_x = rect_x.correct(measurex_Matx);

		//对y坐标进行滤波，与上面对x相同
		KalmanFilterX<2, 2> rect_y(10, 1e-10);
		if (i == 2)
		{
			float Xmin_array[2] = {(float)s_y_last, v_y}; // 初始化 X
			Matx21f Xmin_Matx(Xmin_array);
			rect_y.init(Xmin_Matx, 1e-10);
		}
		else
		{
			rect_y.init(predicts_y, 1e-10);
		}
		float Ay_array[4] = {1, dt, 0, 1};
		Matx22f Ay_Matx(Ay_array);
		rect_y.setA(Ay_Matx);

		float Hy_array[4] = {1, 0, 0, 1};
		Matx22f Hy_Matx(Hy_array);
		rect_y.setH(Hy_Matx);

		float measurey_array[2] = {(float)my_points[4].y, v_y};
		Matx21f measurey_Matx(measurey_array);
		rect_y.predict();
		predicts_y = rect_y.correct(measurey_Matx);

		// 计算最终对p0的预测
		Point p0;
		p0.x = predicts_x(0, 0) + v_x * dt;
		p0.y = predicts_y(0, 0) + v_y * dt;
		circle(src, p0, 2, Scalar(255, 0, 0), -1);
		// 将预测矩形在视频中标出
		Point p1;
		p1.x = p0.x - (my_points[4].x - my_points[0].x);
		p1.y = p0.y - (my_points[4].y - my_points[0].y);
		Point p2;
		p2.x = p0.x - (my_points[4].x - my_points[1].x);
		p2.y = p0.y + (my_points[1].y - my_points[4].y);
		Point p3;
		p3.x = p0.x + (my_points[2].x - my_points[4].x);
		p3.y = p0.y - (my_points[4].y - my_points[2].y);
		Point p4;
		p4.x = p0.x + (my_points[3].x - my_points[4].x);
		p4.y = p0.y + (my_points[3].y - my_points[4].y);
		line(src, p1, p2, Scalar(255, 0, 0), 2);
		line(src, p2, p4, Scalar(255, 0, 0), 2);
		line(src, p4, p3, Scalar(255, 0, 0), 2);
		line(src, p3, p1, Scalar(255, 0, 0), 2);

		imshow("src", src);
		namedWindow("src", 0);
		char key = waitKey(40);
		if (key == 27)
		{
			break;
		}
	}
	capture.release();
	destroyAllWindows();
	return 0;
}