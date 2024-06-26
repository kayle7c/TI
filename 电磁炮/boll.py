import sensor, image, time, math, pyb
from pyb import UART    # 串口包
import ustruct

# 常量设置
RED_THRESHOLD = [(28, 70, 10, 60, -6, 44)]  #阈值设置
red_flag=0

# 外设配置：感光元件设置，时钟设置，串口设置
sensor.reset()
sensor.set_pixformat(sensor.RGB565)  # 灰度图
sensor.set_framesize(sensor.QVGA)    #
sensor.skip_frames(time = 2000)         #跳过2000ms的帧使设置生效
clock = time.clock()
uart = UART(3,115200)
uart.init(115200, bits=8, parity=None, stop=1)


# 串口发送数据
def sending_data(x, y):
    global uart;
    data = ustruct.pack("<bbbbbb",      #格式为俩个字符俩个短整型(2字节)
                   0xA0,
                   int(x // 100),
                   int(x % 100),
                   int(y),
                   0x0D,
                   0x0A
                   )
    uart.write(data);   #必须要传入一个字节数组


while(True):
    clock.tick()
    img = sensor.snapshot().lens_corr(strength = 1.8, zoom = 1.0)
    red_flag=0
    for blob in img.find_blobs(RED_THRESHOLD, pixels_threshold=100, area_threshold=400,marge=True):
        img.draw_cross(blob.cx(), blob.cy())
        p=blob[4]
        print(p)
        #print(blob.cx(),blob.cy())
        sending_data(blob.cx(),blob.cy())
        red_flag=1
    if(red_flag==1):
       sending_data(-1,-1)









