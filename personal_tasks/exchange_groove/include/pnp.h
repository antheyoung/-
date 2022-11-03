#include <iostream>
#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/Core>
#include <opencv2/core/eigen.hpp>
#include <opencv2/opencv.hpp>

struct pnp
{
    float pitch;
    float yaw;
    float roll;
    float distance;
};
double rad2deg(double rad); // 弧度转角度
void get_pnpmessage(cv::Mat &src,std::vector<cv::Point2f> &my_corners,pnp *pnp_message); // 计算位姿数据