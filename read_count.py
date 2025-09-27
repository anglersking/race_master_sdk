import serial
import re
# 修改成你的串口号
# Windows: "COM3"  Linux/Mac: "/dev/ttyUSB0"
PORT = "/dev/tty.usbserial-1110"
BAUD = 115200

def main():
    try:
        ser = serial.Serial(PORT, BAUD, timeout=1)
        print(f"Connected to {PORT} at {BAUD} baud.")
        left_cout=0
        right_cout=0
        left_flag=True
        right_flag=True
        total=0
        last_line=""
        current_line=""


        
        while True:
            line = ser.readline().decode(errors="ignore").strip()
            
            if line:
                # print(line)  # 打印一行 ESP32 发来的数据
                 #用正则提取
                match = re.search(r"ID:\s*(\d+)\s*\|\s*Left Volt:\s*([\d.]+)\s*V\s*\|\s*Right Volt:\s*([\d.]+)\s*V", line)

                if match:
                    id_val = int(match.group(1))
                    left_vol = float(match.group(2))
                    right_vol = float(match.group(3))
                    
                    if left_vol>0.9:
                        if right_cout>0:
                            print("right 无效成绩，双脚经过次数",right_cout)
                            right_cout=0
                        if left_flag:
                            if left_cout>=4:
                                left_cout=0
                            left_cout+=1
                            print(f"左边经过{left_cout}次")
                            
                            left_flag=False

                    if left_vol<0.03:
                        left_flag=True
                    
                    if right_vol>0.9:
                        if left_cout>0:
                            print("left 无效成绩，双脚经过次数",left_cout)

                            left_cout=0
                        if right_flag:
                            if right_cout>=4:
                                right_cout=0
                            right_cout+=1
                            print(f"右边经过{right_cout}次")
                            right_flag=False

                    if right_vol<0.3:
                        right_flag=True

                    if left_cout==4 or right_cout==4:
                        total+=1
                        print(f"有效成绩：{total} 次")

                        if right_cout==4:
                            current_line="right"
                            if last_line==current_line:
                                print("左面没过线重复过右面")

                           
                            last_line=current_line

                                
                            print("右边过的")
                        
                        if left_cout==4:
                            current_line="left"
                            print("左边过的")
                            if last_line==current_line:
                                print("警报 右边没过线重复过左面")
                            last_line=current_line

                        left_cout=0
                        right_cout=0
                                        



                        
                    # if left_cout>2:
                    #         left_cout=0


                        
                    
                    # print("ID:", id_val)
                    # print("左边电压:", left_vol)
                    # print("右边电压:", right_vol)
                
    except KeyboardInterrupt:
        print("\nStopped by user.")
    except Exception as e:
        print(f"Error: {e}")
    finally:
        try:
            ser.close()
        except:
            pass

if __name__ == "__main__":
    main()
