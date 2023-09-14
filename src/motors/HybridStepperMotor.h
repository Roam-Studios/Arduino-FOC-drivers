/**
 *  @file HybridStepperMotor.h
 *
 */

#ifndef HybridStepperMotor_h
#define HybridStepperMotor_h

#include "Arduino.h"
#include "common/base_classes/FOCMotor.h"
#include "common/base_classes/StepperDriver.h"
#include "common/base_classes/Sensor.h"
#include "common/foc_utils.h"
#include "common/time_utils.h"
#include "common/defaults.h"

/**
 Stepper Motor class
*/
class HybridStepperMotor : public FOCMotor
{
public:
    /**
      HybridStepperMotor class constructor
      @param pp  pole pair number
     @param R  motor phase resistance - [Ohm]
     @param KV  motor KV rating (1/K_bemf) - rpm/V
     @param L  motor phase inductance - [H]
    */
    HybridStepperMotor(int pp, float R = NOT_SET, float KV = NOT_SET, float L = NOT_SET);

    /**
     * Function linking a motor and a foc driver
     *
     * @param driver BLDCDriver handle for hardware peripheral control
     */
    void linkDriver(BLDCDriver *driver);

    /**
     * BLDCDriver link:
     */
    BLDCDriver *driver;

    /**  Motor hardware init function */
    void init() override;
    /** Motor disable function */
    void disable() override;
    /** Motor enable function */
    void enable() override;

    /**
     * Function initializing FOC algorithm
     * and aligning sensor's and motors' zero position
     *
     * - If zero_electric_offset parameter is set the alignment procedure is skipped
     *
     * @param zero_electric_offset value of the sensors absolute position electrical offset in respect to motor's electrical 0 position.
     * @param sensor_direction  sensor natural direction - default is CW
     *
     */
    int initFOC(float zero_electric_offset = NOT_SET, Direction sensor_direction = Direction::CW) override;

    /**
     * Function running FOC algorithm in real-time
     * it calculates the gets motor angle and sets the appropriate voltages
     * to the phase pwm signals
     * - the faster you can run it the better Arduino UNO ~1ms, Bluepill ~ 100us
     */
    void loopFOC() override;

    /**
     * Function executing the control loops set by the controller parameter of the HybridStepperMotor.
     *
     * @param target  Either voltage, angle or velocity based on the motor.controller
     *                If it is not set the motor will use the target set in its variable motor.target
     *
     * This function doesn't need to be run upon each loop execution - depends of the use case
     */
    void move(float target = NOT_SET) override;

    float Ua, Ub, Uc; //!< Phase voltages used for inverse Park and Clarke transform

    /**
     * Method using FOC to set Uq to the motor at the optimal angle
     * Heart of the FOC algorithm
     *
     * @param Uq Current voltage in q axis to set to the motor
     * @param Ud Current voltage in d axis to set to the motor
     * @param angle_el current electrical angle of the motor
     */
    void setPhaseVoltage(float Uq, float Ud, float angle_el) override;

private:
    /** Sensor alignment to electrical 0 angle of the motor */
    int alignSensor();
    /** Motor and sensor alignment to the sensors absolute 0 angle  */
    int absoluteZeroSearch();

    // Open loop motion control
    /**
     * Function (iterative) generating open loop movement for target velocity
     * it uses voltage_limit variable
     *
     * @param target_velocity - rad/s
     */
    float velocityOpenloop(float target_velocity);
    /**
     * Function (iterative) generating open loop movement towards the target angle
     * it uses voltage_limit and velocity_limit variables
     *
     * @param target_angle - rad
     */
    float angleOpenloop(float target_angle);
    // open loop variables
    long open_loop_timestamp;
};

#endif
