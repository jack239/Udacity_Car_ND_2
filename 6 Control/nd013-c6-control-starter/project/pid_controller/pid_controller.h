/**********************************************
 * Self-Driving Car Nano-degree - Udacity
 *  Created on: December 11, 2020
 *      Author: Mathilde Badoual
 **********************************************/

#ifndef PID_CONTROLLER_H
#define PID_CONTROLLER_H

class PID {
public:
    /*
    * Constructor
    */
    PID();

    /*
    * Destructor.
    */
    virtual ~PID();

    /*
    * Initialize PID.
    */
    void Init(double Kp, double Ki, double Kd, double output_lim_max, double output_lim_min);

    /*
    * Update the PID error variables given cross track error.
    */
    void UpdateError(double cte);

    /*
    * Calculate the total PID error.
    */
    double TotalError();
  
    /*
    * Update the delta time.
    */
    double UpdateDeltaTime(double new_delta_time);
private:
    double prev_cte;
    bool is_first;


    /*
    * Errors
    */
    double p_error; // current_cte
    double i_error; // prev_cte
    double d_error; // sum

    /*
    * Coefficients
    */
    double Kp_;
    double Ki_;
    double Kd_;



    /*
    * Output limits
    */
    double error_min_;
    double error_max_;


    /*
    * Delta time
    */
    double delta_time_;
};

#endif //PID_CONTROLLER_H


