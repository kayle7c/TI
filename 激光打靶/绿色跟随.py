RED_threshold  = (56, 100, 6, 127, -128, 127)
#RED_threshold  = (86, 100, 3, 127, 6, 127)
#(0, 100, 40, 127, -128, 127)
EXPOSURE_TIME_SCALE = 0.3

import sensor, image, time, math
from pid import PID
from pyb import Pin, Timer, Servo

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.skip_frames(time = 200)
sensor.set_auto_gain(False)
sensor.set_auto_whitebal(False)


clock = time.clock() # Tracks FPS.

# ---------------x - 下面的舵机 - p7    y - 上面的舵机 - p8----------
#pan_pid = PID(p=0.07, i=0, imax=90) #脱机
#tilt_pid = PID(p=0.05, i=0, imax=90) #脱机
pan_pid = PID(p=0.02, i=0, d=0.0000,imax=1)#在线
tilt_pid = PID(p=0.04, i=0, d=0.0000,imax=1)#在线

tim = Timer(4, freq = 50) # Frequency in Hz # arr = 38400 - 1 psc = 125 - 1
pan_servo = tim.channel(1, Timer.PWM, pin = Pin( "P7" ) )
tilt_servo = tim.channel(2, Timer.PWM, pin = Pin( "P8" ) )

blazer_out = Pin('P5', Pin.OUT_PP)
key_in = Pin('P6',Pin.IN)

x=0  #
y=90  #大->下

target_x=148
target_y=97

now_x=0
now_y=0

trace_len=5

def pan_servo_angle_pwm(angle):
    x=angle/180*110
    ccr=(x/180*2+0.5)/20*(tim.period()+1)
    #print(ccr)
    pan_servo.pulse_width(int(ccr))
    #time.sleep_ms(5)
    p=angle
    return p
def tilt_servo_angle_pwm(angle):
    x=angle/180*110
    ccr=(x/180*2+0.5)/20*(tim.period()+1)
    #print(ccr)
    tilt_servo.pulse_width(int(ccr))
    #time.sleep_ms(5)
    t=angle
    return t

#eg : angle = pwm_to_angle (ch1.pulse_width())
def get_angle (ccr):
    #print(ccr)
    angle = (ccr / (tim.period() + 1) * 20 - 0.5) / 2 * 180
    return angle

def find_max(blobs):
    max_size=0
    for blob in blobs:
        if blob[2] * blob[3] > max_size:
            max_blob = blob
            max_size = blob[2] * blob[3]
    return max_blob

def check_dis():
    d=math.sqrt(math.fabs(target_x - now_x) ** 2 + math.fabs(target_y - now_y) ** 2)
    print(d)
    if(d<=trace_len):
        blazer_out.high()

def pause():
    if(key.value()==1):
        while(True)


current_exposure_time_in_microseconds = sensor.get_exposure_us()
sensor.set_auto_exposure(False, \
    exposure_us = int(current_exposure_time_in_microseconds * EXPOSURE_TIME_SCALE))

pan_servo_angle_pwm(x)#下面舵机pa7
tilt_servo_angle_pwm(y)#上面舵机pa8

while(True):
    clock.tick()
    img = sensor.snapshot()
    blobs = img.find_blobs([RED_threshold])
    if blobs:
        max_blob = find_max(blobs)
        now_x=max_blob.cx()
        now_y=max_blob.cy()
        pan_error = max_blob.cx()-148
         #右->+
        tilt_error = max_blob.cy()-97 #下->-
        print("tilt_error: ", tilt_error)
        img.draw_rectangle(max_blob.rect()) # rect
        img.draw_cross(max_blob.cx(), max_blob.cy()) # cx, cy
        pan_output=-pan_pid.get_pid(pan_error,1)
        tilt_output=tilt_pid.get_pid(tilt_error,1)
        print("tilt_output",((y-tilt_output)))
        #if pan_error<5 and pan_error>-5 and tilt_error<5 and tilt_error>-5 :
            #pan_output=0
            #tilt_output=0
            #break
        check_dis()
        x = pan_servo_angle_pwm(x+pan_output)
        y = tilt_servo_angle_pwm(y+tilt_output)
