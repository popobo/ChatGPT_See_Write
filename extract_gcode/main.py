import re

class Point:
    x = 0
    y = 0

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

def remove_three_lines(str):
    lines = str.split('\n')
    del lines[-3:]
    str = '\n'.join(lines)
    return str

def extract_gcode(sp_num_line):
    pattern_xy = r"X([+-]?\d+(\.\d+)?)Y([+-]?\d+(\.\d+)?)"
    pattern_z = r"G1G90 Z([+-]?\d+(\.\d+)?)F8000"
    last_x_pos = 100
    with open("第1页.gcode", "r") as file:
        space_index = 0
        line_index = 0
        start_copy_index = 0
        result = ""
        for line in file:
            line_index += 1
            if start_copy_index != 0 and line_index > start_copy_index:
                result = result + line
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
                    result = remove_three_lines(result)
                    print(result)
                    result = ""
                    print("************* line:", line_index, "space_index:", space_index, " *************")
                    if space_index == sp_num_line:
                        space_index = 0
                        start_copy_index = 0
                last_x_pos = x_pos
        

if __name__ == "__main__":
    # add_space()
    extract_gcode(7)