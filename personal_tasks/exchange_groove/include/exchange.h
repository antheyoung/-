#include <iostream>
#include "opencv2/opencv.hpp"

// 创建兑换槽类
class Exchange
{
public:
    bool find_corners(cv::Mat &src); // 识别四个角点
    cv::Point get_center(cv::Mat &src); // 得出兑换槽的中心
    void draw_contour(cv::Mat &src); // 将兑换槽轮廓框出
    void get_corners(std::vector<cv::Point2f>&my_corners); // 对四个角点进行排序后赋值给my_corners

private:
    std::vector<cv::Point> corners; // 兑换槽的四个端点
    cv::Point center;               // 兑换槽的中心
};