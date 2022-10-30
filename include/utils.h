//
// Created by inory on 2022/10/30.
//

#ifndef AX_MEDIAPIPLE_UTILS_H
#define AX_MEDIAPIPLE_UTILS_H


#include <chrono>
#include "json/json.h"
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

const int color_list[80][3] = {
        //{255 ,255 ,255}, //bg
        {216 , 82 , 24},
        {236 ,176 , 31},
        {125 , 46 ,141},
        {118 ,171 , 47},
        { 76 ,189 ,237},
        {238 , 19 , 46},
        { 76 , 76 , 76},
        {153 ,153 ,153},
        {255 ,  0 ,  0},
        {255 ,127 ,  0},
        {190 ,190 ,  0},
        {  0 ,255 ,  0},
        {  0 ,  0 ,255},
        {170 ,  0 ,255},
        { 84 , 84 ,  0},
        { 84 ,170 ,  0},
        { 84 ,255 ,  0},
        {170 , 84 ,  0},
        {170 ,170 ,  0},
        {170 ,255 ,  0},
        {255 , 84 ,  0},
        {255 ,170 ,  0},
        {255 ,255 ,  0},
        {  0 , 84 ,127},
        {  0 ,170 ,127},
        {  0 ,255 ,127},
        { 84 ,  0 ,127},
        { 84 , 84 ,127},
        { 84 ,170 ,127},
        { 84 ,255 ,127},
        {170 ,  0 ,127},
        {170 , 84 ,127},
        {170 ,170 ,127},
        {170 ,255 ,127},
        {255 ,  0 ,127},
        {255 , 84 ,127},
        {255 ,170 ,127},
        {255 ,255 ,127},
        {  0 , 84 ,255},
        {  0 ,170 ,255},
        {  0 ,255 ,255},
        { 84 ,  0 ,255},
        { 84 , 84 ,255},
        { 84 ,170 ,255},
        { 84 ,255 ,255},
        {170 ,  0 ,255},
        {170 , 84 ,255},
        {170 ,170 ,255},
        {170 ,255 ,255},
        {255 ,  0 ,255},
        {255 , 84 ,255},
        {255 ,170 ,255},
        { 42 ,  0 ,  0},
        { 84 ,  0 ,  0},
        {127 ,  0 ,  0},
        {170 ,  0 ,  0},
        {212 ,  0 ,  0},
        {255 ,  0 ,  0},
        {  0 , 42 ,  0},
        {  0 , 84 ,  0},
        {  0 ,127 ,  0},
        {  0 ,170 ,  0},
        {  0 ,212 ,  0},
        {  0 ,255 ,  0},
        {  0 ,  0 , 42},
        {  0 ,  0 , 84},
        {  0 ,  0 ,127},
        {  0 ,  0 ,170},
        {  0 ,  0 ,212},
        {  0 ,  0 ,255},
        {  0 ,  0 ,  0},
        { 36 , 36 , 36},
        { 72 , 72 , 72},
        {109 ,109 ,109},
        {145 ,145 ,145},
        {182 ,182 ,182},
        {218 ,218 ,218},
        {  0 ,113 ,188},
        { 80 ,182 ,188},
        {127 ,127 ,  0},
};


#define log_info(format, args...) printf("I | %s | %s(%d) | %s | " format, __TIME__, __FILE__, __LINE__, __FUNCTION__, ##args)
#define log_warn(format, args...) printf("W | %s | %s(%d) | %s | " format, __TIME__, __FILE__, __LINE__, __FUNCTION__, ##args)
#define log_error(format, args...) printf("E | %s | %s(%d) | %s | " format, __TIME__, __FILE__, __LINE__, __FUNCTION__, ##args)

unsigned long get_timestamp_in_us();

unsigned long get_timestamp_in_ms();

Json::Value read_json_from_file(const char* filepath);

/**
 * @brief 画框框
 * @tparam T
 * @param bgr
 * @param bboxes
 * @return
 */
template<typename T>
void draw_bboxes(cv::Mat& bgr, const std::vector<T>& bboxes)
{
    int src_w = bgr.cols;
    int src_h = bgr.rows;

    for (size_t i = 0; i < bboxes.size(); i++)
    {
        const T& bbox = bboxes[i];
        cv::Scalar color = cv::Scalar(color_list[bbox.cls][0], color_list[bbox.cls][1], color_list[bbox.cls][2]);
        //fprintf(stderr, "%d = %.5f at %.2f %.2f %.2f %.2f\n", bbox.label, bbox.score,
        //    bbox.x1, bbox.y1, bbox.x2, bbox.y2);

        cv::rectangle(bgr, bbox.bbox, color);

        char text[256];
        sprintf(text, "%s %.1f%%", bbox.label.c_str(), bbox.score * 100);

        int baseLine = 0;
        cv::Size label_size = cv::getTextSize(text, cv::FONT_HERSHEY_SIMPLEX, 0.4, 1, &baseLine);

        int x = bbox.bbox.x;
        int y = bbox.bbox.y - label_size.height - baseLine;
        if (y < 0)
            y = 0;
        if (x + label_size.width > bgr.cols)
            x = bgr.cols - label_size.width;

        cv::rectangle(bgr, cv::Rect(cv::Point(x, y), cv::Size(label_size.width, label_size.height + baseLine)),
                      color, -1);

        cv::putText(bgr, text, cv::Point(x, y + label_size.height),
                    cv::FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(255, 255, 255));
    }
}


/**
 * @brief 画轨迹
 * @tparam T
 * @param bgr
 * @param bboxes
 * @return
 */
template<typename T>
void draw_tracklets(cv::Mat& bgr, const std::vector<T>& tracklets)
{
    int src_w = bgr.cols;
    int src_h = bgr.rows;

    std::vector<typename T::DetType> det_predictions;
    for (const auto& tracklet : tracklets) {
        det_predictions.push_back(tracklet.history.back());
    }

    for (size_t i = 0; i < det_predictions.size(); i++)
    {
        const typename T::DetType& bbox = det_predictions[i];
        cv::Scalar color = cv::Scalar(color_list[bbox.cls][0], color_list[bbox.cls][1], color_list[bbox.cls][2]);
        //fprintf(stderr, "%d = %.5f at %.2f %.2f %.2f %.2f\n", bbox.label, bbox.score,
        //    bbox.x1, bbox.y1, bbox.x2, bbox.y2);

        cv::rectangle(bgr, bbox.bbox, color);

        char text[256];
        sprintf(text, "ID: %d CLASS: %s %.1f%%", tracklets[i].id, bbox.label.c_str(), bbox.score * 100);

        int baseLine = 0;
        cv::Size label_size = cv::getTextSize(text, cv::FONT_HERSHEY_SIMPLEX, 0.4, 1, &baseLine);

        int x = bbox.bbox.x;
        int y = bbox.bbox.y - label_size.height - baseLine;
        if (y < 0)
            y = 0;
        if (x + label_size.width > bgr.cols)
            x = bgr.cols - label_size.width;

        cv::rectangle(bgr, cv::Rect(cv::Point(x, y), cv::Size(label_size.width, label_size.height + baseLine)),
                      color, -1);

        cv::putText(bgr, text, cv::Point(x, y + label_size.height),
                    cv::FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(255, 255, 255));

        // 画轨迹
        std::vector<cv::Point> center_pts = tracklets[i].GetCenterPoints();
        if (center_pts.size() >= 2) {
            for (size_t n = 0; n < center_pts.size() - 1; n++)
                cv::line(bgr, center_pts[n], center_pts[n+1], color, 2);
        }
    }
}

#endif //AX_MEDIAPIPLE_UTILS_H
