//
// Created by inory on 2022/10/30.
//
#include "utils.h"

#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <fcntl.h>

// 最大Post数据长度
#define MAX_POST_DATA_LEN   10 * 1024 // 10KB

using namespace std;

unsigned long get_timestamp_in_us() {
    return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

unsigned long get_timestamp_in_ms() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

Json::Value read_json_from_file(const char* filepath) {
    ifstream ifs(filepath);
    if (!ifs.is_open()) {
        return Json::nullValue;
    }

    Json::Value root;
    ifs >> root;
    return root;
}