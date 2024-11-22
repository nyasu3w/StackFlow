#pragma once

#include <opencv2/opencv.hpp>

namespace detection
{

    typedef struct Object
    {
        cv::Rect_<float> rect;
        int label;
        float prob;
        cv::Point2f landmark[5];
        /* for yolov5-seg */
        cv::Mat mask;
        std::vector<float> mask_feat;
        std::vector<float> kps_feat;
        /* for yolov8-obb */
        float angle;
    } Object;

}