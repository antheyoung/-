#include "energy_m.h"

// 识别周围击打目标
void energy_m::get_edge(Mat &src)
{
    // 通过分离通道使蓝色更明显，去除白色影响
    Mat image = src.clone();
    vector<Mat> channels;
    split(image, channels);
    Mat image_blue = channels.at(0) - channels.at(2);
    // 进行二值化
    Mat thresh;
    threshold(image_blue, thresh, 80, 255, THRESH_BINARY_INV);
    // 通过开运算使图像更明显
    Mat element1 = getStructuringElement(MORPH_RECT, Size(8, 8));
    Mat element2 = getStructuringElement(MORPH_RECT, Size(25, 25));
    Mat thresh_close;
    Mat thresh_open;
    morphologyEx(thresh, thresh_open, MORPH_OPEN, element1);
    // 水漫法将击打目标和其他目标区分
    floodFill(thresh_open, Point(0, 0), Scalar(0));
    // 通过闭运算进一步优化图像
    morphologyEx(thresh_open, thresh_close, MORPH_CLOSE, element2);
    // 寻找轮廓来识别目标
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(thresh_close, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    double area[20] = {0};
    for (int i = 0; i < hierarchy.size(); i++)
    {
        area[i] = contourArea(contours[i]);
        // 通过面积进行限制
        if (area[i] > 800)
        {
            continue;
        }
        // 经过处理后待击打目标变为小方块，使用Point2f数组存储四个点位置
        Point2f rect_points[4];
        RotatedRect rect = minAreaRect(contours[i]);
        rect.points(rect_points);
        // 计算矩形的长和宽以及他们的比例
        int16_t rect_edge1, rect_edge2;
        double radio;
        rect_edge1 = sqrt(pow(rect_points[1].x - rect_points[0].x, 2) + pow(rect_points[1].y - rect_points[0].y, 2));
        rect_edge2 = sqrt(pow(rect_points[2].x - rect_points[1].x, 2) + pow(rect_points[2].y - rect_points[1].y, 2));
        if (rect_edge1 > rect_edge2)
        {
            radio = rect_edge1 / rect_edge2;
        }
        else
        {
            radio = rect_edge2 / rect_edge1;
        }
        // 限制矩形长宽的比例
        if (radio > 1.8)
        {
            continue;
        }
        // 将识别的矩形放入rect_edge容器中
        this->rect_edge.push_back(rect);
    }
}

// 识别能量机关中心
void energy_m::get_center(Mat &src)
{
    // 分离通道，进行二值化
    Mat image = src.clone();
    vector<Mat> channels;
    split(image, channels);
    Mat image_blue = channels.at(0) - channels.at(2);
    Mat thresh_center;
    threshold(image_blue, thresh_center, 80, 255, THRESH_BINARY);
    // 通过膨胀使其他小白块连接在一起，方便识别中心白块
    Mat element_center = getStructuringElement(MORPH_RECT, Size(3, 3));
    dilate(thresh_center, thresh_center, element_center);
    //寻找轮廓
    vector<vector<Point>> contours_center;
    findContours(thresh_center, contours_center, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    for (int j = 0; j < contours_center.size(); j++)
    {
        double area_center = contourArea(contours_center[j]);
        if (area_center < 250 && area_center > 150)
        {
            Rect rect_center = boundingRect(contours_center[j]);
            double radio_center = double(rect_center.width) / double(rect_center.height);
            // 通过比例进行限制
            if (radio_center > 0.8 && radio_center < 1.3)
            {
                this->rect_center = rect_center;
            }
        }
    }
}

// 将周围击打目标用矩形框住
void energy_m::draw_edge(Mat &src)
{
    // 通过迭代器遍历rect_edge
    for (vector<RotatedRect>::iterator it = this->rect_edge.begin(); it != this->rect_edge.end(); it++)
    {
        // 将每一个目标用矩形框出
        Point2f dege_points[4];
        (*it).points(dege_points);
        for (int i = 0; i < 4; i++)
        {
            line(src, dege_points[i], dege_points[(i + 1) % 4], Scalar(0, 0, 255), 2, 8);
        }
    }
}

// 将能量机关中心用矩形框住
void energy_m::draw_center(Mat &src)
{
    rectangle(src, this->rect_center, Scalar(0, 0, 255), 2);
}

// 将中心和击打目标连线
void energy_m::draw_line(Mat &src)
{
    // rect类无法自动获得中心，这里通过计算得出
    Point center;
    center.x = this->rect_center.x + this->rect_center.width / 2;
    center.y = this->rect_center.y + this->rect_center.height / 2;
    circle(src, center, 2, Scalar(0, 0, 255), -1);
    // 通过迭代器遍历rect_edge中心，
    for (vector<RotatedRect>::iterator it = this->rect_edge.begin(); it != this->rect_edge.end(); it++)
    {
        double d = sqrt(pow((*it).center.x - center.x, 2) + pow((*it).center.y - center.y, 2));
        if (d < 150)
        {
            // 将中心和击打目标连线
            line(src, center, (*it).center, Scalar(0, 0, 255), 2);
        }
    }
}

// 计算中心和目标连线与水平线的夹角
float energy_m::get_angle()
{
    // 待击打目标的中心点
    Point center;
    center.x = this->rect_center.x + this->rect_center.width / 2;
    center.y = this->rect_center.y + this->rect_center.height / 2;
    double d;
    uint8_t i = 0;
    float my_angle;
    for (vector<RotatedRect>::iterator it = this->rect_edge.begin(); it != this->rect_edge.end(); it++)
    {
        d = sqrt(pow(it->center.x - center.x, 2) + pow(it->center.y - center.y, 2));
        // 通过长度限制，当识别的不为中心和待击打目标的连线时，角度不进行更新
        if (d > 150 && d < 50)
        {
            return this->angle;
        }
        if ((this->rect_edge.begin())->center.x - (this->rect_center.x + this->rect_center.width / 2) == 0)
        {
            this->angle = 90.000000;
            return 90.000000;
        }
        // 通过斜率，tan和arctan函数计算角度
        float k_line = ((this->rect_edge.begin())->center.y - center.y) / ((this->rect_edge.begin())->center.x - center.x);
        if (k_line < 0)
        {
            k_line = -k_line;
        }
        float line_arctan = atan(k_line);
        my_angle = line_arctan * 180.0 / 3.1415926;
        this->angle = my_angle;
        if (i == 0)
            break;
    }
    return my_angle;
}

// 在视频中标出此时夹角
void energy_m::draw_angle(Mat &src)
{
    putText(src, "angle:", Point(5, 20), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255), 1);
    string angle1 = to_string(this->angle);
    putText(src, angle1, Point(55, 20), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255), 1);
}

// 将夹角转换为与右水平线成0~360角的形式，以便于进行的预测
float energy_m::new_angle()
{
    // 待击打目标中心
    Point center;
    center.x = this->rect_center.x + this->rect_center.width / 2;
    center.y = this->rect_center.y + this->rect_center.height / 2;
    RotatedRect edge_point;
    uint8_t i = 0;
    for (vector<RotatedRect>::iterator it = this->rect_edge.begin(); it != this->rect_edge.end(); it++)
    {
        // 连线在90～180的转换
        if ((*it).center.x<center.x &&this->rect_edge[0].center.y> center.y)
        {
            this->angle = 180.0 - this->angle;
        }
        // 连线在180～270的转换
        if ((*it).center.x < center.x && this->rect_edge[0].center.y < center.y)
        {
            this->angle = 180.0 + this->angle;
        }
        //连线在270～360的转换
        if ((*it).center.x > center.x && this->rect_edge[0].center.y < center.y)
        {
            this->angle = 360.0 - this->angle;
        }
        // 连线成270度角时
        if ((*it).center.x == center.x && this->rect_edge[0].center.y > center.y)
        {
            this->angle = 270.000000;
        }
        // 连线成180度角时
        if ((*it).center.x < center.x && this->rect_edge[0].center.y == center.y)
        {
            this->angle = 180.000000;
        }
        if (i == 0)
            break;
    }
    return this->angle;
}

// 获取能量机关中心点的坐标，因为中心使用较多，增加此函数
Point energy_m::get_center()
{
    Point center;
    center.x = this->rect_center.x + this->rect_center.width / 2;
    center.y = this->rect_center.y + this->rect_center.height / 2;
    return center;
}
