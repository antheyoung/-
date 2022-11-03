#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

// 灯条类
class light_bar
{
public:
    bool find_light(cv::Mat &src, std::vector<cv::Rect> &light_bars);  // 识别灯条
private:
    std::vector<cv::Rect> rect_bars; // rect类型容器用于存储灯条
};