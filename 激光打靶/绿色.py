green_threshold_1 =(88, 100, -128, -12, -128, 127)
green_threshold_2 = (57, 100, -128, -29, 20, 127)
red_threshold_1 = (44, 100, 41, 127, -128, 127)
red_threshold_2 = (40, 100, 3, 127, -128, 127)


import sensor, image, time,math
from pid import PID
from pyb import Pin, Timer, Servo

tim = Timer(4, freq = 50) # Frequency in Hz # arr = 38400 - 1 psc = 125 - 1
ch1 = tim.channel(1, Timer.PWM, pin = Pin( "P7" ) )
ch2 = tim.channel(2, Timer.PWM, pin = Pin( "P8" ) )

EXPOSURE_TIME_SCALE = 0.26

sensor.reset()                      # Reset and initialize the sensor.
sensor.set_pixformat(sensor.RGB565) # Set pixel format to RGB565 (or GRAYSCALE)
sensor.set_framesize(sensor.QVGA)   # Set frame size to QVGA (320x240)

print("Initial exposure == %d" % sensor.get_exposure_us())

sensor.skip_frames(time = 2000)     # Wait for settings take effect.
clock = time.clock()                # Create a clock object to track the FPS.


sensor.set_auto_gain(False)
sensor.set_auto_whitebal(False)
sensor.skip_frames(time = 500)

current_exposure_time_in_microseconds = sensor.get_exposure_us()
print("Current Exposure == %d" % current_exposure_time_in_microseconds)

sensor.set_auto_exposure(False, \
    exposure_us = int(current_exposure_time_in_microseconds * EXPOSURE_TIME_SCALE))

print("New exposure == %d" % sensor.get_exposure_us())

clock = time.clock()

red_locus=[[]]
max_len=5
trace_len=5
x_pid = PID(p=0.01, i=0.0, imax=90)
y_pid = PID(p=0.012, i=0.00 , imax=90)

tim = Timer(4, freq = 50) # Frequency in Hz # arr = 38400 - 1 psc = 125 - 1
ch1 = tim.channel(1, Timer.PWM, pin = Pin( "P7" ) )
ch2 = tim.channel(2, Timer.PWM, pin = Pin( "P8" ) )

def angle_to_pwm_ch1 (angle):
    if angle < 75:
        angle = 75
    if angle > 120:
        angle = 120
    ccr = (angle / 270 * 2 + 0.5) / 20 * (tim.period() + 1) #tim.period是arr
    ch1.pulse_width(int(ccr)) #设置脉冲宽度 -> 角度

def angle_to_pwm_ch2 (angle):
    if angle < 130:
        angle = 130
    if angle > 190:
        angle = 190
    ccr = (angle / 270 * 2 + 0.5) / 20 * (tim.period() + 1) #tim.period是arr
    ch2.pulse_width(int(ccr)) #设置脉冲宽度 -> 角度

def trace_red():
    for blob in img.find_blobs([red_threshold_1],area_threshold=10, pixels_threshold=3, merge=True):
        if(len(red_locus)<max_len):
            red_locus.append([blob.cx(),blob.cy()])
        else :
            if math.sqrt(math.fabs(red_locus[4][0] - blob.cx()) ** 2 + math.fabs(red_locus[4][1] - blob.cy()) ** 2) > trace_len:
                red_locus.pop(0)
                red_locus.append([blob.cx(),blob.cy()])

        img.draw_cross(blob.cx(), blob.cy(),color = (255,0, 255))

def move_green():
    for blob in img.find_blobs([green_threshold_1],area_threshold=10, pixels_threshold=3, merge=True):
        point_x=blob_cx()
        point_y=blob_cy()
        dx=point_x-red_locus[0][0]
        dy=point_y-red_locus[0][1]
        x_output = x_pid.get_pid(dx, 1)
        y_output = y_pid.get_pid(dy, 1)
        angle_to_pwm_ch1(x_output)
        angle_to_pwm_ch2(y_output)

while(True):
    clock.tick()
    img = sensor.snapshot()
    while(True):
        angle_to_pwm_ch1()
        angle_to_pwm_ch2()
    #trace_red()
    #if(len(red_locus)==5):
        #print(red_locus)
        ##for i in red_locus:
        #img.draw_circle(red_locus[1][0],red_locus[1][1],2,color=(255,0,255))
        #move_green()


