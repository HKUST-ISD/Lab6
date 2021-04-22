class PID
{
  float Kp, Ki, Kd;
  float error, last_error;
  float integral;
  float out;
  float int_upper;
 
  public:
  PID(float Kp, float Ki, float Kd, float int_upper) : Kp(Kp), Ki(Ki), Kd(Kd), int_upper(int_upper){}
  
  void update(float state, float desired_state)
  {
    last_error = error;
    error = desired_state - state;
    integral += error;
    if (fabs(integral) > int_upper) integral = integral > 0 ? int_upper : -integral;
    
    out = Kp * error + Ki * integral + Kd * (error - last_error);
  }
  float get_control()
  {
    return out;
  }
};
 
const float control_freq = 20;
float last_time;
PID PID_controller(1.0,0.0,0.0,20);
void setup() {
    Serial.begin(115200);
}
 
void loop() {
  float dt = (micros() - last_time)/ 1.0e6;
  if (dt > 1.0/control_freq)
  {
    int in = analogRead(3);
    
    PID_controller.update(in, 300);
    Serial.print(in);
    Serial.print("  ");
    Serial.print(PID_controller.get_control());
    Serial.print("  ");
    Serial.println(dt);
    last_time = micros();
  }
}