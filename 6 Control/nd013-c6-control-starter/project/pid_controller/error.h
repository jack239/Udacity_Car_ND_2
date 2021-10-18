#ifndef ERROR_H
#define ERROR_H
#include <vector>
#include <Eigen/Core>

struct Error{
    double steer;
    double throttle;
};

inline int get_closest(
        const std::vector<Eigen::Vector2d>& trajectory,
        const Eigen::Vector2d ego_point
        ) {
    double min_dist = std::numeric_limits<double>::infinity();
    int result = -1;
    for (size_t i = 0; i < trajectory.size(); ++i) {
        double dist = (trajectory.at(i) - ego_point).squaredNorm();
        if (dist < min_dist) {
            min_dist = dist;
            result = i;
        }
    }
    return result;
}

inline size_t previous_point(
        const std::vector<Eigen::Vector2d>& trajectory,
        const Eigen::Vector2d ego_point
        ) {
    int closest = get_closest(trajectory, ego_point);
    if (closest <= 0) {
        return 0;
    }
    if (trajectory.size() <= closest + 1) {
        return closest - 1;
    }
    if ((trajectory.at(closest - 1) - ego_point).squaredNorm() <
        (trajectory.at(closest + 1) - ego_point).squaredNorm()) {
        return closest - 1;
    }
    return closest;
}

inline Error get_error(
        const std::vector<double>& x_points,
        const std::vector<double>& y_points,
        const std::vector<double>& v_points,
        double x,
        double y,
        double yaw,
        double velocity
        ) {
    Error result;
    std::vector<Eigen::Vector2d> trajectory;
    trajectory.reserve(x_points.size());
    for (size_t i = 0; i < x_points.size(); ++i) {
        trajectory.emplace_back(x_points[i], y_points[i]);
    }
    Eigen::Vector2d point{x, y};

    int prev_point = previous_point(trajectory, point);
    const auto& prev = trajectory.at(prev_point);
    const auto& next = trajectory.at(prev_point + 1);

    Eigen::Vector2d direction = next - prev;
    double desired_yaw = std::atan2(direction.y(), direction.x());

    std::cout << "\tyaw " << yaw << "\n\tdesired_yaw " <<  desired_yaw << std::endl;
    result.steer = yaw - desired_yaw;
    utils::keep_angle_range_rad(result.steer, -M_PI, M_PI);

    double desired_velocity = v_points.at(prev_point + 1);
    result.throttle = velocity - desired_velocity;
    std::cout << "\tvelocity " << velocity << "\n\tdesired_velocity " <<  desired_velocity << std::endl;

    return result;
}

#endif //ERROR_H
