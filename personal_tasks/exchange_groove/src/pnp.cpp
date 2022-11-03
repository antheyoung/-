#include "pnp.h"

using namespace std;
using namespace cv;

// 弧度转角度
double rad2deg(double rad)
{
    double deg;
    deg = rad / 3.1415926 * 180;
    return deg;
}

// 计算位姿数据
void get_pnpmessage(Mat &src, vector<Point2f> &my_corners, pnp *pnp_message)
{
    // 世界坐标系下四个角点位置
    Matx<Point3f, 4, 1> world_coordinate{
        {-110, 110, 0},
        {110, 110, 0},
        {110, -110, 0},
        {-110, -110, 0}};

    // 相机内参矩阵
    Matx<float, 3, 3> cam_matrix{
        5.9763827661155904e+02, 0., 4.1575511901601089e+02,
        0., 5.9922205940008985e+02, 2.6769310598084320e+02,
        0., 0., 1.};

    // 相机畸变参数
    Matx<float, 5, 1> cam_distortion{
        5.9365728086275861e-02, 6.3271114889939875e-02, 5.5006940318826766e-03, -3.5032524991503678e-03, 0.};
    Mat rvec;                                                                                             // 旋转向量
    Mat tvec;                                                                                             // 位移向量
    solvePnP(world_coordinate, my_corners, cam_matrix, cam_distortion, rvec, tvec, false, SOLVEPNP_IPPE); // 使用solvepnp计算rvec,tvec
    Mat rmat;                                                                                             // 旋转矩阵

    // 旋转向量转化为旋转矩阵
    Rodrigues(rvec, rmat);
    rmat.convertTo(rmat, CV_64FC1);
    tvec.convertTo(tvec, CV_64FC1);

    // 利用Eigen,计算pitch,roll,yaw角和距离
    Eigen::Matrix3d eigen_rmat;
    Eigen::Vector3d eigen_tvec;
    cv2eigen(rmat, eigen_rmat);
    cv2eigen(tvec, eigen_tvec);
    Eigen::Vector3d euler_angles = eigen_rmat.eulerAngles(0, 1, 2);

    // 将结果赋值给pnp_message
    (*pnp_message).pitch = rad2deg(euler_angles[0]);
    (*pnp_message).yaw = rad2deg(euler_angles[1]);
    (*pnp_message).roll = rad2deg(euler_angles[2]);
    (*pnp_message).yaw = sqrt(eigen_tvec.transpose() * eigen_tvec);

    // 在视频中标出相关数据
    Mat world_point = (Mat_<double>(3, 1) << 1, 1, 1);
    Mat my_mat = (Mat_<double>(3, 1));
    my_mat = -(rmat * tvec);
    Point my_center;
    int X = 0, Y = 0;
    // 将四个点的坐标相加再除以4得到中心坐标
    for (vector<Point2f>::iterator it = my_corners.begin(); it != my_corners.end(); it++)
    {
        X += (*it).x;
        Y += (*it).y;
    }
    my_center.x = X / 4;
    my_center.y = Y / 4;
    putText(src, "x:", Point(my_center.x + 20, my_center.y - 20), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0), 1);
    string point_x = to_string(my_mat.at<double>(0, 0));
    putText(src, point_x, Point(my_center.x + 40, my_center.y - 20), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0), 1);
 
    putText(src, "y:", Point(my_center.x + 20, my_center.y - 5), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0), 1);
    string point_y = to_string(my_mat.at<double>(1, 0));
    putText(src, point_y, Point(my_center.x + 40, my_center.y - 5), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0), 1);
   
    putText(src, "z:", Point(my_center.x + 20, my_center.y + 10), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0), 1);
    string point_z = to_string(my_mat.at<double>(2, 0));
    putText(src, point_z, Point(my_center.x + 40, my_center.y + 10), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0), 1);

    putText(src, "pitch:", Point(my_center.x + 20, my_center.y + 25), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0), 1);
    string pitch = to_string(pnp_message->pitch);
    putText(src, pitch, Point(my_center.x + 60, my_center.y + 25), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0), 1);
}