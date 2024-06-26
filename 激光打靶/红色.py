red_threshold_1  = (63, 100, 1, 127, -128, 127)
black_threshold  = (0, 20, -118, -2, -128, 50)
red_threshold_2 = (0, 100, 22, 127, -128, 127)
white_threshold_1 =(28, 100, -128, 127, -128, 127)

import sensor, image, time
from pyb import Pin
from pyb import UART
import ustruct

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


point_x=0
point_y=0

center_x=149
center_y=144

q2corner1=[65,219]
q2corner2=[66,48]
q2corner3=[239,57]
q2corner35=[236,159]
q2corner4=[229,223]

corner1=[1,1]
corner2=[1,1]
corner3=[1,1]
corner4=[1,1]

black_rect=[]
white_rect=[]

black_corner1=[]
black_corner2=[]
black_corner3=[]
black_corner4=[]

white_corner1=[]
white_corner2=[]
white_corner3=[]
white_corner4=[]


clock = time.clock()
uart = UART(3,9600)
uart.init(9600, bits=8, parity=None, stop=1)

key = Pin('P6',Pin.IN)

def sending_data(x1,x2,y1,y2,rx1,rx2,ry1,ry2):
    global uart;
    data = ustruct.pack("<bbbbbbbbbbb",      #格式为俩个字符俩个短整型(2字节)
                   0xA0,
                   x1,
                   x2,
                   y1,
                   y2,
                   rx1,
                   rx2,
                   ry1,
                   ry2,
                   0xA2,
                   0xA3
                   )
    uart.write(data);   #必须要传入一个字节数组

def openmv_receive():
    if uart.any():
        c = uart.readchar()
        return c

def find_point_1(img):
    for blob in img.find_blobs([red_threshold_1],roi=[48,44,202,198],area_threshold=10, pixels_threshold=3, merge=False):
        if blob:
            img.draw_cross(blob.cx(), blob.cy(),color = (255,0, 255)) # cx, cy
            return blob.cx(),blob.cy()
    return 0,0

def find_point_2(img):
    for blob in img.find_blobs([red_threshold_2],roi=[48,44,202,198],area_threshold=5, pixels_threshold=2, merge=True):
        if blob:
            img.draw_cross(blob.cx(), blob.cy(),color = (255,0, 255)) # cx, cy
            return blob.cx(),blob.cy()
    return 0,0


def find_r():
    flag=0
    last_corners=[]
    max_corners=[]
    max_magnitude=0
    fflag=0
    cnt=0
    while(True):
        global corner1,corner2,corner3,corner4,black_rect
        global black_corner1,black_corner2,black_corner3,black_corner4
        clock.tick()
        img = sensor.snapshot()
        for r in img.find_rects(roi=(38,27,2221,207),threshold =20000):
            cnt+=1
            if(cnt>=3):
                fflag=1
            if(fflag==1):
                flag+=1
                if(r.magnitude()>max_magnitude):
                    max_magnitude=r.magnitude()
                    max_corners=list(r.corners())
            img.draw_rectangle(r.rect(), color = (255, 0, 0))
            #print(flag)
            #if(last_corners==r.corners()):
                #if r.magnitude()<=max_magnitude+150 and r.magnitude()>=max_magnitude-150:
                    #flag+=1
                #else:
                    #flag=0
            if(flag==15):
                print(max_corners)
                corner1=list(max_corners[0])
                corner2=list(max_corners[3])
                corner3=list(max_corners[2])
                corner4=list(max_corners[1])
                return
            last_corners = r.corners()

def find_rin():
    last_corners=[]
    while(True):
        global white_corner1,white_corner2,white_corner3,white_corner4
        clock.tick()
        img = sensor.snapshot()
        flag=0
        for blob in img.find_blobs([white_threshold_1],roi=black_rect,area_threshold=3000, pixels_threshold=1000, merge=False):
            img.draw_rectangle(blob.rect(), color = (255, 0, 0))
            white_corner1=blob.corners()[0]
            white_corner2=blob.corners()[1]
            white_corner3=blob.corners()[2]
            white_corner4=blob.corners()[3]
            flag+=1
            img.draw_cross(white_corner1[0], white_corner1[1],color = (255,0, 255))
            img.draw_cross(white_corner2[0], white_corner2[1],color = (255,0, 255))
            img.draw_cross(white_corner3[0], white_corner3[1],color = (255,0, 255))
            img.draw_cross(white_corner4[0], white_corner4[1],color = (255,0, 255))
        #    print(white_corner1,white_corner2,white_corner3,white_corner4)
            #return
        print(flag)

def handle_corner():
    global corner1,corner2,corner3,corner4
    #global black_corner1,black_corner2,black_corner3,black_corner4
    print(black_corner1,black_corner2,black_corner3,black_corner4)
    img.draw_cross(black_corner1[0], black_corner1[1],color = (255,0, 255))
    img.draw_cross(black_corner2[0], black_corner2[1],color = (255,0, 255))
    img.draw_cross(black_corner3[0], black_corner3[1],color = (255,0, 255))
    img.draw_cross(black_corner4[0], black_corner4[1],color = (255,0, 255))
    img.draw_cross(white_corner1[0], white_corner1[1],color = (255,0, 255))
    img.draw_cross(white_corner2[0], white_corner2[1],color = (255,0, 255))
    img.draw_cross(white_corner3[0], white_corner3[1],color = (255,0, 255))
    img.draw_cross(white_corner4[0], white_corner4[1],color = (255,0, 255))


def question1():
    while(True):
        clock.tick()
        img = sensor.snapshot()
        pause()
        choose_task()
        point_x,point_y=find_point_1(img)
        sending_data(point_x//100,point_x%100,point_y//100,point_y%100,center_x//100,center_x%100,center_y//100,center_y%100)
        print(point_x,point_y,center_x,center_y)

def question2():
        condition=0
        while(True):
            clock.tick()
            img = sensor.snapshot()
            pause()
            point_x,point_y=find_point_1(img)
            choose_task()
            if(condition==0 and point_x<=q2corner1[0]+1 and point_x>=q2corner1[0]-1 and point_y<=q2corner1[1]+1 and point_y>=q2corner1[1]-1):
                condition=1
            if(condition==1 and point_x<=q2corner2[0]+1 and point_x>=q2corner2[0]-1 and point_y<=q2corner2[1]+1 and point_y>=q2corner2[1]-1):
                condition=2
            if(condition==2 and point_x<=q2corner3[0]+1 and point_x>=q2corner3[0]-1 and point_y<=q2corner3[1]+1 and point_y>=q2corner3[1]-1):
                condition=3
            if(condition==3 and point_x<=q2corner35[0]+1 and point_x>=q2corner35[0]-1 and point_y<=q2corner35[1]+1 and point_y>=q2corner35[1]-1):
                condition=4
            if(condition==4 and point_x<=q2corner4[0]+1 and point_x>=q2corner4[0]-1 and point_y<=q2corner4[1]+1 and point_y>=q2corner4[1]-1):
                condition=0
            if(condition==0):
                sending_data(point_x//100,point_x%100,point_y//100,point_y%100,q2corner1[0]//100,q2corner1[0]%100,q2corner1[1]//100,q2corner1[1]%100)
            if(condition==1):
                sending_data(point_x//100,point_x%100,point_y//100,point_y%100,q2corner2[0]//100,q2corner2[0]%100,q2corner2[1]//100,q2corner2[1]%100)
            if(condition==2):
                sending_data(point_x//100,point_x%100,point_y//100,point_y%100,q2corner3[0]//100,q2corner3[0]%100,q2corner3[1]//100,q2corner3[1]%100)
            if(condition==3):
                sending_data(point_x//100,point_x%100,point_y//100,point_y%100,q2corner35[0]//100,q2corner35[0]%100,q2corner35[1]//100,q2corner35[1]%100)
            if(condition==4):
                sending_data(point_x//100,point_x%100,point_y//100,point_y%100,q2corner4[0]//100,q2corner4[0]%100,q2corner4[1]//100,q2corner4[1]%100)
            print(point_x,point_y,q2corner1,q2corner2,q2corner3,q2corner4)
            print("now is 2")

def question3():
        find_r()
     #   find_rin()
        condition=0
        cnt=0
        while(True):
            clock.tick()
            img = sensor.snapshot()
            #pause()
            point_x,point_y=find_point_2(img)
            choose_task()
            if(condition==0 and point_x<=corner1[0]+2 and point_x>=corner1[0]-2 and point_y<=corner1[1]+2 and point_y>=corner1[1]-2):
                cnt+=1
            if(condition==1 and point_x<=corner2[0]+2 and point_x>=corner2[0]-2 and point_y<=corner2[1]+2 and point_y>=corner2[1]-2):
                cnt+=1
            if(condition==2 and point_x<=corner3[0]+2 and point_x>=corner3[0]-2 and point_y<=corner3[1]+2 and point_y>=corner3[1]-2):
                cnt+=1
            if(condition==3 and point_x<=corner4[0]+2 and point_x>=corner4[0]-2 and point_y<=corner4[1]+2 and point_y>=corner4[1]-2):
                cnt+=1
            if(condition==0 and cnt==1):
                condition=1
                cnt=0
            if(condition==1 and cnt==1):
                condition=2
                cnt=0
            if(condition==2 and cnt==1):
                condition=3
                cnt=0
            if(condition==3 and cnt==1):
                condition=0
                cnt=0

            if(condition==0):
                sending_data(point_x//100,point_x%100,point_y//100,point_y%100,corner1[0]//100,corner1[0]%100,corner1[1]//100,corner1[1]%100)
            if(condition==1):
                sending_data(point_x//100,point_x%100,point_y//100,point_y%100,corner2[0]//100,corner2[0]%100,corner2[1]//100,corner2[1]%100)
            if(condition==2):
                sending_data(point_x//100,point_x%100,point_y//100,point_y%100,corner3[0]//100,corner3[0]%100,corner3[1]//100,corner3[1]%100)
            if(condition==3):
                sending_data(point_x//100,point_x%100,point_y//100,point_y%100,corner4[0]//100,corner4[0]%100,corner4[1]//100,corner4[1]%100)
            print("now is 3")
            img.draw_cross(corner1[0],corner1[1],size=2,color = (255,0,0))
            img.draw_cross(corner2[0],corner2[1],size=2,color = (255,0,0))
            img.draw_cross(corner3[0],corner3[1],size=2,color = (255,0,0))
            img.draw_cross(corner4[0],corner4[1],size=2,color = (255,0,0))

def calibration():
    while(True):
        global center_x,center_y,q2corner1,q2corner2,q2corner3,q2corner35,q2corner4
        img = sensor.snapshot()
        received_number=openmv_receive()
        if(received_number==1):
            q2corner1[0],q2corner1[1]=find_point_1(img)
            print(q2corner1)
        if(received_number==2):
            q2corner2[0],q2corner2[1]=find_point_1(img)
            print(q2corner2)
        if(received_number==3):
            q2corner3[0],q2corner3[1]=find_point_1(img)
            print(q2corner3)
        if(received_number==4):
            q2corner35[0],q2corner35[1]=find_point_1(img)
            print(q2corner35)
        if(received_number==5):
            q2corner4[0],q2corner4[1]=find_point_1(img)
            print(q2corner4)
        if(received_number==6):
            center_x,center_y=find_point_1(img)
            print(center_x,center_y)
            return


def choose_task():
    received_number=openmv_receive()
    if(received_number==0):
        calibration()
    if(received_number==1):
        question1()
    if(received_number==2):
        question2()
    if(received_number==3 or received_number==4):

        question3()

def pause():
    t=0
    if(key.value()==1):
        while(True):
            t+=1

while(True):
    while(True):
        print(key.value())
    choose_task()
