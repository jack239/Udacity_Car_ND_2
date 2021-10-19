#ifndef ERROR_H
#define ERROR_H
#include <vector>
#include <Eigen/Core>

struct Error{
    double steer = 0;
    double throttle = 0;
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

inline size_t target_index(
        const std::vector<Eigen::Vector2d>& trajectory,
        const Eigen::Vector2d ego_point
) {
    int closest = get_closest(trajectory, ego_point);
    return std::min<int>(closest + 1, trajectory.size() - 1);
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

    if (x_points.size() < 2) {
        return result;
    }

    std::vector<Eigen::Vector2d> trajectory;
    trajectory.reserve(x_points.size());
    for (size_t i = 0; i < x_points.size(); ++i) {
        trajectory.emplace_back(x_points[i], y_points[i]);
    }
    Eigen::Vector2d point{x, y};

    int t_index = target_index(trajectory, point);

    int dir_index = std::min<int>(x_points.size() - 1, t_index + 2);
    Eigen::Vector2d direction = trajectory.at(dir_index) - point;
    double desired_yaw = std::atan2(direction.y(), direction.x());

    result.steer = yaw - desired_yaw;
    result.steer = utils::keep_angle_range_rad(result.steer, -M_PI, M_PI);

    double desired_velocity = v_points.at(t_index);
    result.throttle = velocity - desired_velocity;

    return result;
}

#endif //ERROR_H
