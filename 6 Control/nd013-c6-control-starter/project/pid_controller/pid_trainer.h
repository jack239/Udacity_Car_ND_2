#ifndef PID_TRAINER_H
#define PID_TRAINER_H
#include "pid_controller.h"
#include "json.hpp"

class PIDTRainer {
private:
    static constexpr double update_time = 10;
    enum class State {
        Init,
        Inc,
        Dec
    };
public:
    using Params = std::array<double, 3>;

    PIDTRainer(
            PID& pid,
            const Params& init_params,
            const Params& init_delta,
            const std::string& pid_name,
            double lim_min,
            double lim_max
    )
            : pid_(pid),
              state_(State::Init),
              best_params_{init_params},
              current_params_{init_params},
              delta_{init_delta},
              error_min_(lim_min),
              error_max_(lim_max),
              pid_name_(pid_name)
    {
        set_params();
    }
public:
    void update_cte(double cte, double delta_time) {
        current_error_ = cte * cte * delta_time;
        time_ += delta_time;
        if (time_ > update_time) {
            update_state();
        }
    }

    ~PIDTRainer() {
        plot_best();
    }
private:
    void set_params() {
        std::cout << pid_name_ << " set_params " << nlohmann::json(current_params_) <<  std::endl;
        pid_.Init(current_params_[0], current_params_[1], current_params_[2], error_min_, error_max_);
        current_error_ = 0;
        time_ = 0;
    }

    void update_state() {
        //        std::cout << "update_state current "<< sum_cte << "best " << best_sum_cte << std::endl;
        if (state_ == State::Init) {
            param_id_ = 0;
            max_error_ = std::numeric_limits<double>::infinity();
            try_params(State::Inc);
        } else {
            double aver_error = current_error_ / time_;
            if (max_error_ > aver_error) {
                best_params_ = current_params_;
                plot_best();
                delta_[param_id_] *= 1.1;
                max_error_ = aver_error;
                try_params(State::Inc);
            } else {
                if (state_ == State::Inc) {
                    try_params(State::Dec);
                } else {
                    delta_[param_id_] *= 0.9;
                    //                     delta[param_id] *= 0.1;
                    try_params(State::Inc);
                }
            }
        }
    }

    void try_params(State new_state) {
        current_params_ = best_params_;
        state_ = new_state;
        time_ = 0;
        if (state_ == State::Inc) {
            param_id_ = (param_id_ + 1) % 3;
            current_params_[param_id_] += delta_[param_id_];
            plot_best();
        }
        set_params();
    }

    void plot_best() const {
        std::cout << "=======================================" << std::endl;
        std::cout << "best params for " << pid_name_ <<  " is " << nlohmann::json(current_params_) <<  std::endl;
        std::cout << "=======================================" << std::endl;
    }

private:
    PID& pid_;
    State state_;
    size_t param_id_;
    Params best_params_;
    Params current_params_;
    Params delta_;

    double current_error_;
    double max_error_;

    double error_min_;
    double error_max_;
    std::string pid_name_;
    double time_;
};
#endif //PID_TRAINER_H
