import sensor, image, time, math, pyb
from pyb import UART    # 串口包
import ustruct

# 常量设置
BOLL_THRESHOLD = [(170,255)]  #阈值设置
BLACK_THRESHOLD = [(0, 40)]  #阈值设置

# 外设配置：感光元件设置，时钟设置，串口设置
sensor.reset()
sensor.set_pixformat(sensor.GRAYSCALE)  # 灰度图
sensor.set_framesize(sensor.QVGA)    #
sensor.skip_frames(time = 2000)         #跳过2000ms的帧使设置生效
clock = time.clock()
uart = UART(3,115200)
uart.init(115200, bits=8, parity=None, stop=1)


# 串口发送数据
def sending_data(x1, x2):
    global uart;
    data = ustruct.pack("<bbbbb",      #格式为俩个字符俩个短整型(2字节)
                   0xA0,
                   x1,
                   x2,
                   0x0D,
                   0x0A
                   )
    uart.write(data);   #必须要传入一个字节数组


while(True):
    clock.tick()
    img = sensor.snapshot().lens_corr(strength = 1.8, zoom = 1.0)
    #x1 =
    #y1 =
    #x2 =
    x1 = 20
    y1 = 14
    x2 = 221
    y2 = 206
  #  y2 = 228
    #for blob in img.find_blobs(BLACK_THRESHOLD, pixels_threshold=10000, area_threshold=10000):
        #x1 = blob.x()
        #y1 = blob.y()
        #x2 = blob.x() + blob.w()
        #y2 = blob.y() + blob.h()
        #img.draw_rectangle(blob.rect())
    for blob in img.find_blobs(BLACK_THRESHOLD, pixels_threshold=35, area_threshold=35, roi=[x1, y1, x2, y2]):
        if blob.elongation() < 0.4:
            if blob.w()<200 and blob.h()<200 and blob.w()>2 and blob.h()>2:
                img.draw_rectangle(blob.rect())
                img.draw_cross(blob.cx(), blob.cy())
                img.draw_keypoints([(blob.cx(), blob.cy(), int(math.degrees(blob.rotation())))], size=20)
                sending_data(blob.cx() // 100, blob.cx() % 100)
                print(blob.cx() // 100, blob.cx() % 100)
        else :
            #print('a')
            sending_data(-1, -1)
    #print(clock.fps())





