#include <iostream>
#include <opencv2/opencv.hpp>

#include "energy_m.h"
#include "KalmanFilterX.hpp"

using namespace std;
using namespace cv;

int main()
{
    VideoCapture capture;
    VideoWriter writer;
    capture.open("../energy_mechanism.avi");
    writer.open("识别结果.avi",capture.get(CAP_PROP_FOURCC),30,Size(capture.get(CAP_PROP_FRAME_WIDTH),capture.get(CAP_PROP_FRAME_HEIGHT)),true);
    if (!capture.isOpened())
    {
        cout << "can not find ..." << endl;
        return -1;
    }
    Mat src;
    uint8_t num = 0;
    // dt为间隔时间
    const float dt = 0.5;
    float v, last_angle, predict_angle;
    float sum;
    while (capture.read(src))
    {
        // 创建my_energy_m对象
        energy_m my_energy_m;
        Matx21f xwhat;
        // 第一次滤波的初始值为测量值
        if (num == 1)
        {
            last_angle = my_energy_m.new_angle();
        }
        // 第二次以及以后滤波的初始值为上次的最优估计值
        if (num > 1)
        {
            last_angle = xwhat(0, 0);
        }

        // 图像识别部分
        my_energy_m.get_edge(src);
        my_energy_m.get_center(src);
        my_energy_m.draw_center(src);
        my_energy_m.draw_edge(src);
        my_energy_m.draw_line(src);
        my_energy_m.get_angle();
        my_energy_m.new_angle();
        my_energy_m.draw_angle(src);

        // 在第二帧图像后进行卡尔曼滤波
        if (num != 0)
        {
            // 每五帧计算一次角速度
            float sub_angle = my_energy_m.new_angle() - last_angle;
            if (sub_angle < 0)
            {
                sub_angle = -sub_angle;
            }
            if (sub_angle > 50)
            {
                num == 0;
            }
            if (num % 5 == 0)
            {
                sum += sub_angle;
                v = sub_angle / dt;
            }

            // 创建角度的卡尔曼滤波对象
            KalmanFilterX<2, 2> angle_kalman(1e-2, 1e-9);
            // 卡尔曼滤波初始化，Xmin为先验估计
            float Xmin_array[2] = {last_angle, v};
            Matx21f Xmin_Matx(Xmin_array);
            angle_kalman.init(Xmin_Matx, 1e-8);
            // 赋值状态转换模型A
            float A_array[4] = {1, dt, 0, 1};
            Matx22f A_Matx(A_array);
            angle_kalman.setA(A_Matx);
            // 赋值状态变量到测量值的转换矩阵H
            float H_array[4] = {1, 0, 0, 1};
            Matx22f H_Matx(H_array);
            angle_kalman.setH(H_Matx);
            // 调用卡尔曼滤波的预测函数计算先验误差协方差 （此时不应该进行预测，KalmanFilterX文件先进行了预测）
            angle_kalman.predict();
            // 给出测量值，在此测量值角度为识别的角度，v无其他传感器提供数据，一直为计算的速度
            float Z_array[2] = {my_energy_m.new_angle(), v};
            Matx21f Z_Matx(Z_array);
            xwhat = angle_kalman.correct(Z_Matx);
            // xwhat为当前最优估计，还没有进行预测
            if (xwhat(0, 0) > 360)
            {
                xwhat(0, 0) = xwhat(0, 0) - 360;
            }
            // 进行预测
            predict_angle = xwhat(0, 0) + v * dt / 25;
            if (predict_angle > 360)
            {
                predict_angle = predict_angle - 360;
            }
            // 将预测的结果在视频中标出
            Point predict_point;
            predict_point.x = my_energy_m.get_center().x + 125 * cos(predict_angle / 180 * 3.1415926);
            predict_point.y = my_energy_m.get_center().y + 125 * sin(predict_angle / 180 * 3.1415926);
            line(src, my_energy_m.get_center(), predict_point, Scalar(0, 255, 255), 2);
        }
        num++;
        imshow("src", src);
        namedWindow("src", 0);
        char key = waitKey(50);
        if (key == 27)
        {
            break;
        }
    }
    capture.release();
    destroyAllWindows();
    return 0;
}