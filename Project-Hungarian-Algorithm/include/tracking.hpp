#pragma once
#include "assignment.hpp"
#include <string>
#include <vector>

namespace tracking {
struct Box{double cx{},cy{},width{},height{};};
struct Detection{Box box;int truth_id;int truncation=0;int occlusion=0;}; // Truth is used only by the evaluator.
struct Frame{int index;std::vector<Detection> detections;};
struct Association {
    std::vector<int> detection_of_track;
    lap::Cost objective{};
};
double iou(const Box&a,const Box&b);
lap::Matrix augmented_cost(const std::vector<Box>&tracks,const std::vector<Box>&detections,
    const std::string&metric,lap::Cost penalty=600000,double distance_gate=80.0,double iou_gate=0.05);
Association associate(const std::vector<Box>&tracks,const std::vector<Box>&detections,
    const std::string&metric,const std::string&method,lap::Cost penalty=600000,
    double distance_gate=80.0,double iou_gate=0.05);
std::vector<Frame> synthetic_sequence(int population,int frame_count,std::uint64_t seed,
    const std::string&scenario);
std::vector<Frame> read_kitti(const std::string&path); // Local training labels, Car only.
}
