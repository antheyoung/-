#include "light_bar.h"
using namespace std;
using namespace cv;

bool light_bar::find_light(Mat &src, vector<Rect> &light_bars)
{
    this->rect_bars.reserve(10);
    Mat image = src;
    //进行二值化、高斯模糊使图像更明显
    Mat gray;
    cvtColor(image, gray, COLOR_BGR2GRAY);
    Mat gaussian;
    GaussianBlur(gray, gaussian, Size(3, 3), 0);
    Mat thresh;
    threshold(gaussian, thresh, 150, 255, 0);
    //寻找所有轮廓
    vector<vector<Point>> contours;
    findContours(thresh, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    for (int i = 0; i < contours.size(); i++)
    {
        double area = contourArea(contours[i]);
        //通过面积进行限制
        if (area > 200)
        {
            Rect rect = boundingRect(contours[i]);
            float radio = double(rect.width) / double(rect.height);
            //通过比例进行限制，将符合条件的灯条存储
            if (radio > 2.5 || radio < 0.5)
            {
                this->rect_bars.push_back(rect);
            }
        }
    }
    // 灯条数量不足两个一定不为装甲板
    if (this->rect_bars.size() < 2)
    {
        // cout << "can not find light_bar ..." << endl;
        return false;
    }
    light_bars = rect_bars;
    this->rect_bars = {};
    return true;
}
