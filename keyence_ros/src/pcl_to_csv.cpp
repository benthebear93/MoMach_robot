#include "ros/ros.h"
#include "std_msgs/Float32.h"
#include <iostream>
//pcl related
#include <pcl/point_types.h>
#include <pcl/point_cloud.h>
#include <pcl/ros/conversions.h>
#include <pcl/io/pcd_io.h>
#include <pcl_ros/point_cloud.h>
#include <sensor_msgs/PointCloud2.h>
#include <vector>
using namespace std;

struct xyz
{
    double x,y,z;
};
typedef pcl::PointCloud<pcl::PointXYZ> Cloud;
const static std::string DEFAULT_FRAME_ID = "lj_v7080_optical_frame";
class Surface{
    public:

        Surface();
        ~Surface();
        bool init();
        double pos_y  = 0;
        double temp   = 0;
        std::string frame_id;
        std::string tf_frame;
        Cloud::Ptr  point3d{new Cloud};
        vector<xyz> point_storage;

    private:
        ros::NodeHandle nh_, pnh_{"~"};

        ros::Publisher  point_pub_;
        ros::Subscriber posy_sub_;
        ros::Subscriber cloud_sub_;
        
        void pos_cb(const std_msgs::Float32::ConstPtr& msg);
        void cloud_cb(const sensor_msgs::PointCloud2& msg);
};

Surface::Surface()
{
    ROS_INFO("Surface Node init");
    auto ret = init();
    ROS_ASSERT(ret);
}
Surface::~Surface()
{
    ros::shutdown();
}

bool Surface::init()
{
    point_pub_ = nh_.advertise<sensor_msgs::PointCloud2>("CMM_profile", 100);
    posy_sub_ = nh_.subscribe("posy", 100, &Surface::pos_cb, this);
    cloud_sub_ = nh_.subscribe("/profiles", 1, &Surface::cloud_cb, this);

    pos_y = 0;
    temp = 0;
    return true;
}
void Surface::pos_cb(const std_msgs::Float32::ConstPtr& msg)
{
    pos_y = msg->data;
}

void Surface::cloud_cb(const sensor_msgs::PointCloud2& msg)
{
    //std::cout << "msg is :" << typeid(msg).name() << std::endl;
    pcl::PCLPointCloud2 pcl_pc;  // pcl point cloud
    pcl_conversions::toPCL(msg, pcl_pc);  // sensor msg to pcl
    pcl::PointCloud<pcl::PointXYZ> input_cloud; // pcl point xyz
    pcl::fromPCLPointCloud2(pcl_pc, input_cloud); // pcl to Pointcloud 

    pnh_.param<std::string>("frame_id", tf_frame, std::string("/base_link"));

    //point3d.reset(new Cloud);
    point3d->header.frame_id  = tf_frame;
    point3d->is_dense         = false;
    point3d->width            = 16;
    point3d->height           = 1;

    point3d -> points.clear();
    point3d->points.reserve(32);
    double x = 0,y = 0,z =0;

    for(int i =0; i<16; i++){
        //std::cout <<"input cloud" << input_cloud.points[i] << std::endl;
        x = input_cloud.points[i].x;
        y = pos_y;
        z = input_cloud.points[i].z;
        point3d->points.push_back(pcl::PointXYZ(x, y, z));
        //std::cout <<"input cloud" << point3d->points[i] << std::endl;
        if(y!=temp)
        {
            point_storage.push_back({x,y,z});
            cout << "vector size :" << point_storage.size()<<endl;
            if(i==15)
            {
                temp = pos_y;
            }
        }
    }
    point_pub_.publish(point3d);
}

int main(int argc, char**argv)
{
    std_msgs::Float32 mid_laser;
    ros::init(argc, argv, "pcl_to_csv");
    Surface surface;
    ros::spin ();
}