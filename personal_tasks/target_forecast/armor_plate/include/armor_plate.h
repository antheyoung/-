#pragma once
#include <opencv2/opencv.hpp>

using namespace cv;

// 装甲板类
class armor_plate
{
public:
    bool get_bar(std::vector<Rect> &light_bars); // 识别灯条
    bool is_armor(); // 判断是否为装甲板，还是只是两个灯条
    void draw_rect(Mat &src); // 在视频中标出装甲板的矩形
    void give_point(Point my_points[5]); // 将装甲板的四个点和中心赋值给数组

private:
    Point points[5]; // 装甲板矩形四个点和中心
    Rect armor_bar[2];  // 装甲板的两个灯条矩形
};

// void armor_dectect(cv::Mat &src, vector<Point> Points);