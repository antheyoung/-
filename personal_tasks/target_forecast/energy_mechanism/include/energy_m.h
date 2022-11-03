#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
using namespace std;
using namespace cv;

// 创建能量机关类
class energy_m
{
public: 
    void get_edge(Mat &src); // 识别周围击打目标
    void get_center(Mat &src); // 识别能量机关中心
    void draw_edge(Mat &src); // 将周围击打目标用矩形框住
    void draw_center(Mat &src); // 将能量机关中心用矩形框住
    void draw_line(Mat &src); // 将中心和击打目标连线
    float get_angle(); // 计算中心和目标连线与水平线的夹角
    void draw_angle(Mat &src); // 在视频中标出此时夹角
    float new_angle(); // 将夹角转换为与右水平线成0~360角的形式
    Point get_center(); // 获取能量机关中心点的坐标，因为中心使用较多，增加此函数

private:
    vector<RotatedRect> rect_edge; // 能量机关四周的待击打目标
    Rect rect_center; // 能量机关中心
    float angle; // 中心和目标连线与水平线的夹角
};
