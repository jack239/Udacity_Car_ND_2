/**********************************************
 * Self-Driving Car Nano-degree - Udacity
 *  Created on: December 11, 2020
 *      Author: Mathilde Badoual
 **********************************************/

#include "pid_controller.h"
#include "utils.h"
#include <vector>
#include <iostream>
#include <math.h>

using namespace std;

PID::PID() {}

PID::~PID() {}

void PID::Init(double Kpi, double Kii, double Kdi, double output_lim_mini, double output_lim_maxi) {
    std::cout << "PID::Init " << Kpi << " " << Kii << " "  << Kdi << " " << output_lim_maxi << " " <<  output_lim_mini << std::endl;
    Kp_ = Kpi;
    Ki_ = Kii;
    Kd_ = Kdi;
    p_error = i_error = d_error = 0;
    is_first = true;
    error_min_ = output_lim_mini;
    error_max_ = output_lim_maxi;
}


void PID::UpdateError(double cte) {
    if (delta_time_ < 1e-6) {
        return;
    }
    if (!is_first) {
        d_error = (cte - p_error) / delta_time_;
    } else {
        is_first = false;
        d_error = 0;
    }
    p_error = cte;
    i_error += cte * delta_time_;
}

double PID::TotalError() {
    double error = -(Kp_ * p_error + Kd_ * d_error + Ki_ * i_error);
    std::cout << "error " << error << std::endl;
    std::cout << "lims "  << error_min_  << " " <<  error_max_ << std::endl;
    return utils::clampD(error, error_min_, error_max_);
}

double PID::UpdateDeltaTime(double new_delta_time) {
    delta_time_ = new_delta_time;
    return delta_time_;
}
