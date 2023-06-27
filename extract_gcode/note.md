# 分割提取每一个字对应的gcode
- 将每一个字都调整为从原点开始绘制

# 设计sqlite数据库
- 有哪些字段
    - id ch(utf8字符) gcode(字符串) max_x(整数) max_y(整数) min_x(整数) min_y(整数)