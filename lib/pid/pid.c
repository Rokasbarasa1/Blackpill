#include "./pid.h"

/**
 * @brief Initialize pid configuration and store it in struct
 * 
 * @param gain_proportional 
 * @param gain_integral 
 * @param gain_derivative 
 * @param desired_value value that you want to achieve
 * @param time time since system start in microseconds
 * @return struct pid 
 */
struct pid pid_init(
    float gain_proportional, 
    float gain_integral, 
    float gain_derivative, 
    float desired_value,
    uint64_t time,
    float max_value,
    float min_value,
    uint8_t stop_windup
){
    struct pid new_pid;

    new_pid.m_gain_proportional = gain_proportional;
    new_pid.m_gain_integral = gain_integral;
    new_pid.m_gain_derivative = gain_derivative;
    new_pid.m_integral_sum = 0;
    new_pid.m_last_error = 0;
    new_pid.m_desired_value = desired_value;
    new_pid.m_previous_time = time;
    new_pid.m_max_value = max_value;
    new_pid.m_min_value = min_value;
    new_pid.m_stop_windup = stop_windup;
    new_pid.m_last_proportional_error = 0;
    new_pid.m_last_integral_error = 0;
    new_pid.m_last_derivative_error = 0;

    return new_pid;
}


/**
 * @brief Calculate the error based on the configuration of the pid and the new value
 * 
 * @param pid_instance pid config
 * @param value current value that the error will be calculated for 
 * @param time time since system start in microseconds
 * @return float error result
 */
float pid_get_error(struct pid* pid_instance, float value, uint64_t time){

    float error_p = 0, error_i = 0, error_d = 0;

    float error = (pid_instance->m_desired_value - value);

    float elapsed_time_sec = ((float)time-(float)pid_instance->m_previous_time)/1000000.0;

    // proportional
    {
        error_p = error;
    }

    // integral
    {
        pid_instance->m_integral_sum += (error * elapsed_time_sec);

        if(pid_instance->m_stop_windup == 1){
            // clamp the integral if it is getting out of bounds
            if((pid_instance->m_integral_sum * pid_instance->m_gain_integral) > pid_instance->m_max_value){
                pid_instance->m_integral_sum = pid_instance->m_max_value / pid_instance->m_gain_integral;
            }else if(pid_instance->m_integral_sum * pid_instance->m_gain_integral < pid_instance->m_min_value){
                pid_instance->m_integral_sum = pid_instance->m_min_value / pid_instance->m_gain_integral;
            }
        }
        error_i = pid_instance->m_integral_sum;
    }

    // derivative
    {
        // divide by the time passed
        error_d = (error_p - pid_instance->m_last_error) / elapsed_time_sec;

        // Dont let it get out of bounds 
        if(error_d > pid_instance->m_max_value){
            error_d = pid_instance->m_max_value;
        }else if(error_d < pid_instance->m_min_value){
            error_d = pid_instance->m_min_value;
        }
        
        // set the previous error for the next iteration
        pid_instance->m_last_error = error;
    }


    pid_instance->m_last_proportional_error = pid_instance->m_gain_proportional * error_p;
    pid_instance->m_last_integral_error = pid_instance->m_gain_integral * error_i;
    pid_instance->m_last_derivative_error = pid_instance->m_gain_derivative * error_d;

    // end result
    float total_error = (pid_instance->m_gain_proportional * error_p) + 
                (pid_instance->m_gain_integral * error_i) + 
                (pid_instance->m_gain_derivative * error_d) ;

    // save the time for next calculation
    pid_instance->m_previous_time = time;
    return total_error;
}


/**
 * @brief Calculate the error based on the configuration of the pid and the new value
 * 
 * @param pid_instance pid config
 * @param error your own calculated error that will be used to get pid error
 * @param time time since system start in microseconds
 * @return float error result
 */
float pid_get_error_own_error(struct pid* pid_instance, float error, uint64_t time){

    float error_p = 0, error_i = 0, error_d = 0;

    float elapsed_time_sec = ((float)time-(float)pid_instance->m_previous_time)/1000.0;

    // proportional
    {
        error_p = error;
    }

    // integral
    {
        pid_instance->m_integral_sum += (error * elapsed_time_sec);

        if(pid_instance->m_stop_windup == 1){
            // clamp the integral if it is getting out of bounds
            if((pid_instance->m_integral_sum * pid_instance->m_gain_integral) > pid_instance->m_max_value){
                pid_instance->m_integral_sum = pid_instance->m_max_value / pid_instance->m_gain_integral;
            }else if(pid_instance->m_integral_sum * pid_instance->m_gain_integral < pid_instance->m_min_value){
                pid_instance->m_integral_sum = pid_instance->m_min_value / pid_instance->m_gain_integral;
            }
        }
        error_i = pid_instance->m_integral_sum;
    }

    // derivative
    {
        // divide by the time passed
        error_d = (error_p - pid_instance->m_last_error) / elapsed_time_sec;

        // Dont let it get out of bounds 
        if(error_d > pid_instance->m_max_value){
            error_d = pid_instance->m_max_value;
        }else if(error_d < pid_instance->m_min_value){
            error_d = pid_instance->m_min_value;
        }
        
        // set the previous error for the next iteration
        pid_instance->m_last_error = error;
    }

    // end result
    float total_error = (pid_instance->m_gain_proportional * error_p) + 
                (pid_instance->m_gain_integral * error_i) + 
                (pid_instance->m_gain_derivative * error_d) ;

    // save the time for next calculation
    pid_instance->m_previous_time = time;
    return total_error;
}

/**
 * @brief Change the desired value. Mainly for user inputs for pid
 * 
 * @param pid_instance pid config
 * @param value new desired value
 */
void pid_set_desired_value(struct pid* pid_instance, float value){
    pid_instance->m_desired_value = value;
}

void pid_set_proportional_gain(struct pid* pid_instance, float proportional_gain){
    pid_instance->m_gain_proportional = proportional_gain;
}

void pid_set_integral_gain(struct pid* pid_instance, float integral_gain){
    pid_instance->m_gain_integral = integral_gain;
}

void pid_set_derivative_gain(struct pid* pid_instance, float derivative_gain){
    pid_instance->m_gain_derivative = derivative_gain;
}

void pid_reset_integral_sum(struct pid* pid_instance){
    pid_instance->m_integral_sum = 0;
}

void pid_set_previous_time(struct pid* pid_instance, uint64_t time){
    pid_instance->m_previous_time = time;
}

float pid_get_last_proportional_error(struct pid* pid_instance){
    return pid_instance->m_last_proportional_error;
}

float pid_get_last_integral_error(struct pid* pid_instance){
    return pid_instance->m_last_integral_error;
}

float pid_get_last_derivative_error(struct pid* pid_instance){
    return pid_instance->m_last_derivative_error;
}