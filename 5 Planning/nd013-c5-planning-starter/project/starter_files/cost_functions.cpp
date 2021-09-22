/**********************************************
 * Self-Driving Car Nano-degree - Udacity
 *  Created on: September 20, 2020
 *      Author: Munir Jojo-Verge
 **********************************************/

/**
 * @file cost_functions.cpp
 **/

#include "cost_functions.h"
#include <Eigen/Core>

using namespace std;

namespace cost_functions {
// COST FUNCTIONS

double diff_cost(vector<double> coeff, double duration,
                 std::array<double, 3> goals, std::array<float, 3> sigma,
                 double cost_weight) {
  /*
  Penalizes trajectories whose coordinate(and derivatives)
  differ from the goal.
  */
  double cost = 0.0;
  vector<double> evals = evaluate_f_and_N_derivatives(coeff, duration, 2);
  //////////////cout << "26 - Evaluating f and N derivatives Done. Size:" <<
  /// evals.size() << endl;

  for (size_t i = 0; i < evals.size(); i++) {
    double diff = fabs(evals[i] - goals[i]);
    cost += logistic(diff / sigma[i]);
  }
  ////////////cout << "diff_coeff Cost Calculated " << endl;
  return cost_weight * cost;
}

double collision_circles_cost_spiral(const std::vector<PathPoint>& spiral,
                                     const std::vector<State>& obstacles) {
  bool collision{false};
  auto n_circles = CIRCLE_OFFSETS.size();

  for (auto wp : spiral) {
    if (collision) {
      // LOG(INFO) << " ***** COLLISION DETECTED *********" << std::endl;
      break;
    }
    Eigen::Vector2d wp_pos{wp.x, wp.y};
    Eigen::Vector2d wp_dir{std::cos(wp.theta), std::sin(wp.theta)};

    for (size_t c = 0; c < n_circles && !collision; ++c) {
      // Circle placement: Where should the circles be at? The code below
      // is NOT complete. HINT: use CIRCLE_OFFSETS[c], sine and cosine to
      // calculate x and y: cur_y + CIRCLE_OFFSETS[c] * std::sin/cos(cur_yaw)
      Eigen::Vector2d circle_center = wp_pos + CIRCLE_OFFSETS[c] * wp_dir;

      for (auto obst : obstacles) {
        if (collision) {
          break;
        }
        auto actor_yaw = obst.rotation.yaw;
        Eigen::Vector2d actor_pos{obst.location.x, obst.location.y};
        Eigen::Vector2d actor_dir{std::cos(actor_yaw), std::sin(actor_yaw)};

        for (size_t c2 = 0; c2 < n_circles && !collision; ++c2) {
          Eigen::Vector2d  actor_center = actor_pos + CIRCLE_OFFSETS[c2] * actor_dir;
          // Distance from circles to obstacles/actor: How do you calculate
          // the distance between the center of each circle and the
          // obstacle/actor
          double dist = (actor_center - circle_center).norm();  // <- Update

          collision = (dist < (CIRCLE_RADII[c] + CIRCLE_RADII[c2]));
        }
      }
    }
  }
  return (collision) ? COLLISION : 0.0;
}

double close_to_main_goal_cost_spiral(const std::vector<PathPoint>& spiral,
                                      State main_goal) {
  // The last point on the spiral should be used to check how close we are to
  // the Main (center) goal. That way, spirals that end closer to the lane
  // center-line, and that are collision free, will be prefered.

  // distance between last point on spiral and main goal: How do we
  // calculate the distance between the last point on the spiral (spiral[n-1])
  // and the main goal (main_goal.location). Use spiral[n - 1].x, spiral[n -
  // 1].y and spiral[n - 1].z.
  // Use main_goal.location.x, main_goal.location.y and main_goal.location.z
  // Ex: main_goal.location.x - spiral[n - 1].x
  Eigen::Vector3d spiral_end{
      spiral.back().x, 
      spiral.back().y, 
      spiral.back().z, 
  };
  Eigen::Vector3d goal_pos{
      main_goal.location.x,
      main_goal.location.y,
      main_goal.location.z,
  };

  auto dist = (goal_pos - spiral_end).norm();

  auto cost = logistic(dist);
  // LOG(INFO) << "distance to main goal: " << dist;
  // LOG(INFO) << "cost (log): " << cost;
  return cost;
}
}  // namespace cost_functions
