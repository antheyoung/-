#include "exchange.h"
using namespace std;
using namespace cv;

// 识别四个角点
bool Exchange::find_corners(Mat &src)
{
    Mat image = src.clone();
    // 视频中白色影响较大，分离通道去除白色影响
    vector<Mat> channels;
    split(image, channels);
    Mat image_blue = channels.at(0) - channels.at(2);
    // 进行二值化
    Mat thresh;
    threshold(image_blue, thresh, 120, 255, THRESH_BINARY);
    // 右上角点有一些识别断裂处，通过闭运算使图像的角点更完整
    Mat element = getStructuringElement(MORPH_RECT, Size(12, 12));
    Mat thresh_close;
    morphologyEx(thresh, thresh_close, MORPH_CLOSE, element);
    //寻找角点轮廓
    vector<vector<Point>> contours;
    findContours(thresh_close, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    for (int i = 0; i < contours.size(); i++)
    {
        Rect rect = boundingRect(contours[i]);
        float radio = (float)rect.width / (float)rect.height;
        // 要识别多种情况，对面积限制不合适，使用长宽比例进行进行限制
        if (radio < 1.4 && radio > 0.7)
        {
            // 将每个角点的中心存入corners容器
            Point center;
            center.x = rect.x + rect.width / 2;
            center.y = rect.y + rect.height / 2;
            this->corners.push_back(center);
        }
    }
    if (this->corners.size() < 4)
    {
        cout << "未识别到四个角点 " << endl;
        return false;
    }
    return true;
}

// 将兑换槽轮廓框出
void Exchange::draw_contour(Mat &src)
{    
    // 第coner个点和第i个点为对角位置
    int corner = 0;
    int i = -1;
    if (this->corners.size() == 4)
    {
        for (vector<Point>::iterator it = this->corners.begin(); it != this->corners.end(); it++)
        {
            int dx = (int)this->corners.begin()->x - (int)(*it).x;
            int dy = (int)this->corners.begin()->y - (int)(*it).y;
            i++;
            if (dx > 30 && dy > 30)
            {
                corner = i;
            }
        }
    }
    // 将i点与另外两个点相连.corer与另外两个点相连
    if (corner == 1)
    {
        line(src, this->corners[0], this->corners[2], Scalar(0, 0, 255), 1);
        line(src, this->corners[0], this->corners[3], Scalar(0, 0, 255), 1);
        line(src, this->corners[corner], this->corners[2], Scalar(0, 0, 255), 1);
        line(src, this->corners[corner], this->corners[3], Scalar(0, 0, 255), 1);
    }
    if (corner == 2)
    {
        line(src, this->corners[0], this->corners[1], Scalar(0, 0, 255), 1);
        line(src, this->corners[0], this->corners[3], Scalar(0, 0, 255), 1);
        line(src, this->corners[corner], this->corners[1], Scalar(0, 0, 255), 1);
        line(src, this->corners[corner], this->corners[3], Scalar(0, 0, 255), 1);
    }
    if (corner == 3)
    {
        line(src, this->corners[0], this->corners[2], Scalar(0, 0, 255), 1);
        line(src, this->corners[0], this->corners[1], Scalar(0, 0, 255), 1);
        line(src, this->corners[corner], this->corners[2], Scalar(0, 0, 255), 1);
        line(src, this->corners[corner], this->corners[1], Scalar(0, 0, 255), 1);
    }
}

// 得出兑换槽的中心
Point Exchange::get_center(Mat &src)
{
    Point my_center;
    int X, Y;
    // 将四个点的坐标相加再除以4得到中心坐标
    for (vector<Point>::iterator it = this->corners.begin(); it != this->corners.end(); it++)
    {
        X += (*it).x;
        Y += (*it).y;
    }
    my_center.x = X / 4;
    my_center.y = Y / 4;
    circle(src, my_center, 3, Scalar(0, 0, 255), -1);
    return my_center;
}

// 对四个角点进行排序后赋值给my_corners
void Exchange::get_corners(vector<Point2f> &my_corners)
{
    Point corner_points[4];
    int i = 0;
    // 将数据赋值给corner_points
    for (vector<Point>::iterator it = this->corners.begin(); it != this->corners.end(); it++)
    {
        corner_points[i++] = (*it);
    }
    // 通过循环得出右下角的角点corner_poins[big_num]
    int big_num;
    for (int j = 0; j < 4; j++)
    {
        int d1_x = corner_points[j].x - corner_points[0].x;
        int d1_y = corner_points[j].y - corner_points[0].y;
        int d2_x = corner_points[j].x - corner_points[1].x;
        int d2_y = corner_points[j].y - corner_points[1].y;
        int d3_x = corner_points[j].x - corner_points[2].x;
        int d3_y = corner_points[j].y - corner_points[2].y;
        int d4_x = corner_points[j].x - corner_points[3].x;
        int d4_y = corner_points[j].y - corner_points[3].y;
        if (d1_x > -30 && d1_y > -30 && d2_x > -30 && d2_y > -30 && d3_x > -30 && d3_y > -30 && d4_x > -30 && d4_y > -30)
        {
            big_num = j;
            break;
        }
    }
    // 通过判断和右下角角点的相对位置确定其他角点下标,并按顺序重新赋值给my_points
    Point my_points[4];
    my_points[2] = corner_points[big_num];
    for (int j = 0; j < 4; j++)
    {
        if (corner_points[j].x - corner_points[big_num].x < -30 &&
            corner_points[j].y - corner_points[big_num].y < 30 &&
            corner_points[j].y - corner_points[big_num].y > -30)
        {
            my_points[3] = corner_points[j];
        }
        if (corner_points[j].x - corner_points[big_num].x > -30 &&
            corner_points[j].x - corner_points[big_num].x < 30 &&
            corner_points[j].y - corner_points[big_num].y < -30)
        {
            my_points[1] = corner_points[j];
        }
        if (corner_points[j].x - corner_points[big_num].x < -30 &&
            corner_points[j].y - corner_points[big_num].y < -30)
        {
            my_points[0] = corner_points[j];
        }
    }
    // 再次将数据存储在my_corners容器中
    for (int j = 0; j < 4; j++)
    {
        my_corners.push_back(my_points[j]);
    }
}