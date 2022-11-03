#include <iostream>
#include <armor_plate.h>
using namespace std;
using namespace cv;

// 识别灯条
bool armor_plate::get_bar(vector<Rect> &light_bars)
{
    // 两个灯条组成装甲板 sdoo
    if (light_bars.size() != 2)
    {
        cout << light_bars.size() << endl;
        //cout << "have too many bars" << endl; // sdoo 待优化
        return false;
    }
    // 获取矩形的四个端点
    this->armor_bar[0] = light_bars[0];
    this->armor_bar[1] = light_bars[1];
    Point point1 = Point(armor_bar[0].x + armor_bar[0].width / 2, armor_bar[0].y);
    Point point2 = Point(armor_bar[0].x + armor_bar[0].width / 2, armor_bar[0].y + armor_bar[0].height);
    Point point3 = Point(armor_bar[1].x + armor_bar[1].width / 2, armor_bar[1].y);
    Point point4 = Point(armor_bar[1].x + armor_bar[1].width / 2, armor_bar[1].y + armor_bar[1].height);
    // 将矩形四个端点按照左上，左下，右上，右下的顺序存储
    if (point2.x < point4.x)
    {
        this->points[0] = point1;
        this->points[1] = point2;
        this->points[2] = point3;
        this->points[3] = point4;
    }
    else
    {
        this->points[0] = point3;
        this->points[1] = point4;
        this->points[2] = point1;
        this->points[3] = point2;
    }
    // points[4]为装甲板矩形的中心
    this->points[4].x = ((point1.x + point3.x) / 2 + (point2.x + point4.x) / 2) / 2;
    this->points[4].y = ((point2.y + point1.y) / 2 + (point3.y + point4.y) / 2) / 2;
    return true;
}

// 判断是否为装甲板，还是只是两个灯条
bool armor_plate::is_armor()
{
    //通过判断两个灯条是否平行判断目标是否为真正的装甲板，d1为装甲板两端的距离差
    int d1 = ((this->points[0].x - this->points[2].x) - (this->points[1].x - this->points[3].x));
    if (d1 > 0)
    {
        if (d1 > 50)
        {
            cout << "this is not a armor" << endl;
            return false;
        }
    }
    else
    {
        d1 = -d1;
        if (d1 > 50)
        {
            cout << "this is not a armor" << endl;
            return false;
        }
    }
    return true;
}

// 在视频中标出装甲板的矩形
void armor_plate::draw_rect(Mat &src)
{
    line(src, points[0], points[1], Scalar(0, 0, 255), 2);
    line(src, points[1], points[3], Scalar(0, 0, 255), 2);
    line(src, points[3], points[2], Scalar(0, 0, 255), 2);
    line(src, points[2], points[0], Scalar(0, 0, 255), 2);
    circle(src, points[4], 2, Scalar(0, 0, 255), -1);
}

// 将装甲板的四个点和中心赋值给数组
void armor_plate::give_point(Point my_points[5])
{
    my_points[0] = this->points[0];
    my_points[1] = this->points[1];
    my_points[2] = this->points[2];
    my_points[3] = this->points[3];
    my_points[4] = this->points[4];
}
