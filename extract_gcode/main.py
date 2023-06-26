import re
import serial
import time
import sys

class Point:
    x = 0
    y = 0

class Matrix:
    left_top = Point()
    left_down = Point()
    right_top = Point()
    right_down = Point()

def sleep_ms(ms):
    start_time = time.perf_counter()
    while (time.perf_counter() - start_time) * 1000 < ms:
        pass

class Extract_gcode:
    ser = None

    def __init__(self) -> None:
        self.ser = serial.Serial("COM4", 115200, stopbits=1, parity="N")

    def add_space():
        str = ""
        with open("常用汉字库3500.txt", "r", encoding="utf8") as file:
            for line in file:
                chars = list(line)
                i = 0
                for char in chars[1:]:
                    str += char
                    str += "  "
                    i += 1
                    if i == 8:
                        str += "\n"
                        i = 0
        with open("char_with_space.txt", "w", encoding="utf8") as file:
            file.write(str)

    def remove_three_lines(self, gcode: str):
        lines = gcode.split('\n')
        del lines[-3:]
        gcode = '\n'.join(lines)
        return gcode

    def ser_write(self, gcode: str):
        lines = gcode.split("\n")
        for line in lines:
            line += "\n"
            b_line = line.encode()
            self.ser.write(b_line)
            sleep_ms(25)

    def wash_gcode(self, gcodes: list[str]):
        pattern_xy = r"X([+-]?\d+(\.\d+)?)Y([+-]?\d+(\.\d+)?)"
        new_gcode = ""
        for gcode in gcodes:
            origin_x = 0
            origin_y = 0
            lines = gcode.split("\n")
            for line in lines:
                match_xy = re.search(pattern_xy, line)
                if match_xy:
                    origin_x = float(match_xy.group(1))
                    origin_y = float(match_xy.group(3))
                    break

            for line in lines:
                match_xy = re.search(pattern_xy, line)
                if match_xy:
                    x_pos = float(match_xy.group(1))
                    y_pos = float(match_xy.group(3))
                    # print("old: ", x_pos, " ", y_pos)
                    x_pos = round(x_pos - origin_x, 3)
                    y_pos = round(y_pos - origin_y, 3)
                    # print("new: ", x_pos, " ", y_pos)
                    line = re.sub(r'X[-+]?\d+(\.\d+)?', f'X{x_pos:.3f}', line)
                    line = re.sub(r'Y[-+]?\d+(\.\d+)?', f'Y{y_pos:.3f}', line)
                    new_gcode = new_gcode + line + "\n"
            self.ser_write(new_gcode)
            new_gcode = ""


    def extract_gcode(self, sp_num_line: int):
        pattern_xy = r"X([+-]?\d+(\.\d+)?)Y([+-]?\d+(\.\d+)?)"
        pattern_z = r"G1G90 Z([+-]?\d+(\.\d+)?)F8000"
        last_x_pos = sys.maxsize
        gcodes = []
        with open("第1页.gcode", "r") as file:
            space_index = 0
            line_index = 0
            start_copy_index = 0
            word_gcode = ""
            for line in file:
                line_index += 1
                if start_copy_index != 0 and line_index > start_copy_index:
                    word_gcode = word_gcode + line
                match_xy = re.search(pattern_xy, line)
                if match_xy:
                    x_pos = float(match_xy.group(1))
                    y_pos = float(match_xy.group(3))
                    if space_index == 0:
                        start_copy_index = line_index
                    if (x_pos - last_x_pos > 10):
                        space_index += 1
                        # start to copy another gcode segment
                        start_copy_index = line_index
                        word_gcode = self.remove_three_lines(word_gcode)
                        gcodes.append(word_gcode)
                        self.wash_gcode(gcodes)
                        word_gcode = ""
                        if space_index == sp_num_line:
                            space_index = 0
                            start_copy_index = 0
                    last_x_pos = x_pos
        

if __name__ == "__main__":
    # G92 X0 Y0 Z0 设置原点
    # G1G90 Z0.5F8000 抬笔
    # G1G90 Z6.0F8000 落笔
    eg = Extract_gcode()
    # gcode = "G1G90 Z0.5F8000" + "\n" + "G1G90 Z6.0F8000"
    # eg.ser_write(gcode)
    gcode = "G92 X0 Y0 Z0\n"
    eg.ser_write(gcode)
    eg.extract_gcode(7)
    