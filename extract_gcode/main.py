import re
import serial
import time
import sys
import sqlite3
import os

PAGE_NUM = 21
FIRST_USEFUL_LINE = 7

class Point:
    x = 0
    y = 0

class Matrix:
    left_top = Point()
    left_down = Point()
    right_top = Point()
    right_down = Point()
    center = Point()

class GcodeObj:
    ch = ""
    gcode = ""
    max_x = -sys.maxsize - 1
    max_y = -sys.maxsize - 1
    min_x = sys.maxsize
    min_y = sys.maxsize
    center_x = 0
    center_y = 0

def sleep_ms(ms):
    start_time = time.perf_counter()
    while (time.perf_counter() - start_time) * 1000 < ms:
        pass

class Extract_gcode:
    ser :serial.Serial = None
    conn : sqlite3.Connection = None

    def __init__(self):
        try:
            self.ser = serial.Serial("COM4", 115200, stopbits=1, parity="N")
        except serial.SerialException as e:
            print(f"Failed to open serial port: {e}")

        if os.path.exists("gcode.sqlite"):
            os.remove("gcode.sqlite")

        self.conn = sqlite3.connect("gcode.sqlite")
        self.conn.execute('''
        CREATE TABLE IF NOT EXISTS gcode (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            ch TEXT,
            gcode TEXT,
            max_x FLOAT,
            max_y FLOAT,
            min_x FLOAT,
            min_y FLOAT,
            center_x FLOAT,
            center_y FLOAT
        );
        ''')

    def insert_gcode(self, gcode_obj: GcodeObj):
        sql = '''INSERT INTO gcode (ch, gcode, max_x, max_y, min_x, min_y, center_x, center_y) VALUES (?, ?, ?, ?, ?, ?, ?, ?)'''
        data = (gcode_obj.ch, gcode_obj.gcode, gcode_obj.max_x, gcode_obj.max_y, gcode_obj.min_x, gcode_obj.min_y, gcode_obj.center_x, gcode_obj.center_y)
        cursor = self.conn.cursor()
        cursor.execute(sql, data)
        self.conn.commit()
        cursor.close()

    def add_space(self):
        str = ""
        with open("chinese3500_ascii.txt", "r", encoding="utf8") as file:
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

    def remove_some_lines(self, gcode: str, num):
        lines = gcode.split('\n')
        del lines[-num:]
        gcode = '\n'.join(lines)
        return gcode

    def ser_write(self, gcode: str):
        if self.ser == None:
            return
        lines = gcode.split("\n")
        for line in lines:
            line += "\n"
            b_line = line.encode()
            self.ser.write(b_line)
            sleep_ms(25)

    def wash_gcode(self, gcode_obj: GcodeObj):
        pattern_xy = r"X([+-]?\d+(\.\d+)?)Y([+-]?\d+(\.\d+)?)"
        new_gcodes = []
        origin_x = 0
        origin_y = 0
        lines = gcode_obj.gcode.split("\n")
        new_gcode = ""
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
                
                # print("origin_x:", origin_x, "x_pos: ", x_pos)
                assert (x_pos < 25 and x_pos > -25), "x_pos error"
                assert (y_pos < 25 and y_pos > -25), "y_pos error"

                gcode_obj.max_x = max(gcode_obj.max_x, x_pos)
                gcode_obj.max_y = max(gcode_obj.max_y, y_pos)
                gcode_obj.min_x = min(gcode_obj.min_x, x_pos)
                gcode_obj.min_y = min(gcode_obj.min_y, y_pos)
                gcode_obj.center_x = round((gcode_obj.max_x + gcode_obj.min_x) / 2, 3)
                gcode_obj.center_y = round((gcode_obj.max_y + gcode_obj.min_y) / 2, 3)
                
                # print("new: ", x_pos, " ", y_pos)
                line = re.sub(r'X[-+]?\d+(\.\d+)?', f'X{x_pos:.3f}', line)
                line = re.sub(r'Y[-+]?\d+(\.\d+)?', f'Y{y_pos:.3f}', line)
                new_gcode = new_gcode + line + "\n"
            else:
                new_gcode = new_gcode + line + "\n"
        gcode_obj.gcode = new_gcode
        # self.ser_write(new_gcode)

    def extract_gcode(self, sp_num_line: int):
        chs = ""

        with open("chinese3500_ascii.txt", "r", encoding="utf8") as file:
            chs = file.readline()

        ch_index = 1
        for i in range(1, PAGE_NUM + 1):
            if (i == 21):
                pass
            filename = "第" + str(i) + "页.gcode"
            with open(filename, "r") as file:
                pattern_xy = r"X([+-]?\d+(\.\d+)?)Y([+-]?\d+(\.\d+)?)"
                last_x_pos = sys.maxsize
                space_index = 0
                line_index = 0
                start_copy_index = FIRST_USEFUL_LINE
                gcode_obj = GcodeObj()
                lines = file.readlines()
                num_of_lines = len(lines)
                for line in lines:
                    line_index += 1
                    if line_index >= start_copy_index:
                        gcode_obj.gcode = gcode_obj.gcode + line
                    match_xy = re.search(pattern_xy, line)
                    if match_xy:
                        x_pos = float(match_xy.group(1))

                        # new ch
                        if x_pos - last_x_pos > 10 or last_x_pos - x_pos > 75:
                            # add current gcode_obj to database
                            if last_x_pos == sys.maxsize:
                                last_x_pos = x_pos
                                continue
                            gcode_obj.gcode = self.remove_some_lines(gcode_obj.gcode, 3)
                            self.wash_gcode(gcode_obj)
                            gcode_obj.ch = chs[ch_index]
                            self.insert_gcode(gcode_obj)
                            # new gcode_obj
                            gcode_obj = GcodeObj()
                            start_copy_index = line_index
                            gcode_obj.gcode += line
                            ch_index += 1
                            space_index += 1
                            # new ch line and new ch
                            if last_x_pos != sys.maxsize and last_x_pos - x_pos > 75:
                                assert space_index == 8 , "space_index != 8" 
                                space_index = 0
                                gcode_obj.gcode += line
                        last_x_pos = x_pos
                    if line_index == num_of_lines and i == PAGE_NUM:
                        gcode_obj.gcode = self.remove_some_lines(gcode_obj.gcode, 4)
                        self.wash_gcode(gcode_obj)
                        gcode_obj.ch = chs[ch_index]
                        self.insert_gcode(gcode_obj)
                    
                    
        

if __name__ == "__main__":
    # G92 X0 Y0 Z0 设置原点
    # G1G90 Z0.5F8000 抬笔
    # G1G90 Z6.0F8000 落笔
    eg = Extract_gcode()
    # gcode = "G1G90 Z0.5F8000" + "\n" + "G1G90 Z6.0F8000"
    # eg.ser_write(gcode)
    # gcode = "G92 X0 Y0 Z0\n"
    # eg.ser_write(gcode)
    eg.extract_gcode(7)