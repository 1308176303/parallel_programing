import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import os
import sys
import io
import matplotlib as mpl
from matplotlib.font_manager import FontProperties

# 强制设置标准输出编码为UTF-8
if sys.platform.startswith('win'):
    sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8')

# 查找合适的中文字体
def find_chinese_font():
    """查找系统中可用的中文字体"""
    # Windows常见中文字体路径
    potential_fonts = [
        'C:/Windows/Fonts/simhei.ttf',    # 黑体
        'C:/Windows/Fonts/simsun.ttc',    # 宋体
        'C:/Windows/Fonts/simkai.ttf',    # 楷体
        'C:/Windows/Fonts/msyh.ttc',      # 微软雅黑
    ]
    
    # 检查字体是否存在并返回第一个找到的字体
    for font_path in potential_fonts:
        if os.path.exists(font_path):
            return FontProperties(fname=font_path)
    
    # 如果找不到具体字体文件，返回系统中可能的中文字体名称
    return FontProperties(family=['SimHei', 'Microsoft YaHei', 'SimSun'])

# 设置中文字体和图表风格
def setup_plot_style():
    """设置图表风格和中文字体支持"""
    # 使用白色主题
    plt.style.use('default')
    
    # 设置图表背景和网格样式
    plt.rcParams.update({
        'axes.facecolor': '#f0f0f0',        # 浅灰色背景
        'figure.facecolor': 'white',        # 白色图形背景
        'grid.color': '#e0e0e0',            # 浅灰色网格
        'grid.linestyle': '--',             # 虚线网格
        'grid.linewidth': 0.8,              # 网格线宽度
        'axes.grid': True,                  # 默认显示网格
        'figure.dpi': 100,                  # 图像DPI
        'axes.unicode_minus': False,        # 正确显示负号
    })

    # 尝试使用中文字体
    chinese_font = find_chinese_font()
    return chinese_font

# 定义鲜艳的色彩方案
def get_vibrant_colors():
    """返回一组鲜艳的颜色"""
    return {
        'blue': '#1E88E5',      # 亮蓝色
        'red': '#E53935',       # 鲜红色
        'green': '#43A047',     # 鲜绿色
        'purple': '#8E24AA',    # 紫色
        'orange': '#FB8C00',    # 橙色
        'teal': '#00897B',      # 蓝绿色
        'pink': '#D81B60',      # 粉色
        'yellow': '#F9A825'     # 黄色
    }

def jichu_sum(csv_file='jichu_sum.csv', chinese_font=None):
    """处理基础求和算法的CSV数据并生成可视化图像 - 针对2的幂次方数据优化"""
    # 读取CSV文件
    df = pd.read_csv(csv_file)
    
    # 获取鲜艳的颜色
    colors = get_vibrant_colors()
    
    # 设置图像大小
    plt.figure(figsize=(12, 7))
    
    # 绘制两种加速比曲线
    plt.plot(df['数组大小'], df['两路链式加速比'], label='两路链式加速比', 
             marker=None, linestyle='-', linewidth=2.5, 
             color=colors['blue'], alpha=0.9)
    plt.plot(df['数组大小'], df['递归加速比'], label='递归加速比', 
             marker=None, linestyle='-', linewidth=2.5, 
             color=colors['red'], alpha=0.9)
    
    # 识别2的幂次方数值
    power_of_two = []
    power_values = []
    for i in range(7, 26):  # 2^7 到 2^25
        power = 2**i
        power_of_two.append(power)
        power_values.append(i)
    
    # 找出数据中最接近2的幂次方的点
    mark_points_blue = []
    mark_points_red = []
    
    for power in power_of_two:
        # 找到最接近的数据点
        closest_idx = (df['数组大小'] - power).abs().idxmin()
        mark_points_blue.append((df['数组大小'].iloc[closest_idx], df['两路链式加速比'].iloc[closest_idx]))
        mark_points_red.append((df['数组大小'].iloc[closest_idx], df['递归加速比'].iloc[closest_idx]))
    
    # 标记2的幂次方点
    x_blue = [p[0] for p in mark_points_blue]
    y_blue = [p[1] for p in mark_points_blue]
    x_red = [p[0] for p in mark_points_red]
    y_red = [p[1] for p in mark_points_red]
    
    plt.plot(x_blue, y_blue, 'o', markersize=6, color=colors['blue'], alpha=0.8)
    plt.plot(x_red, y_red, 'x', markersize=7, color=colors['red'], alpha=0.8)
    
    # 使用中文字体设置标题和标签
    if chinese_font is not None:
        plt.title('基础求和算法加速比对比', fontproperties=chinese_font, fontsize=18, fontweight='bold', pad=15)
        plt.xlabel('数组大小 (2的幂次方)', fontproperties=chinese_font, fontsize=14, fontweight='bold')
        plt.ylabel('加速比', fontproperties=chinese_font, fontsize=14, fontweight='bold')
        # 设置图例中的中文字体
        legend = plt.legend(fontsize=12, framealpha=0.9, loc='best')
        for text in legend.get_texts():
            text.set_fontproperties(chinese_font)
    else:
        plt.title('基础求和算法加速比对比', fontsize=18, fontweight='bold', pad=15)
        plt.xlabel('数组大小 (2的幂次方)', fontsize=14, fontweight='bold')
        plt.ylabel('加速比', fontsize=14, fontweight='bold')
        plt.legend(fontsize=12, framealpha=0.9, loc='best')
    
    # 对数刻度设置
    plt.xscale('log', base=2)  # 使用以2为底的对数刻度
    
    # 设置主要刻度为2的幂次方
    major_ticks = [2**i for i in range(7, 26)]
    plt.xticks(major_ticks, ['2^{}'.format(i) for i in range(7, 26)], rotation=45)
    
    # 调整刻度显示 - 根据数据范围显示部分标签
    if len(major_ticks) > 10:
        # 仅保留部分主要刻度标签，避免重叠
        visible_ticks = []
        visible_labels = []
        for i, (tick, label) in enumerate(zip(major_ticks, ['2^{}'.format(i) for i in range(7, 26)])):
            if i % 2 == 0 or i == len(major_ticks) - 1:  # 每隔一个显示，首尾必显示
                visible_ticks.append(tick)
                visible_labels.append(label)
        plt.xticks(visible_ticks, visible_labels, rotation=45)
    
    # 添加缓存临界点垂直线和注释
    cache_points = [
        (64*1024, "L1缓存临界点 (~512KB)"),
        (1024*1024, "L2缓存临界点 (~8MB)"), 
        (2*1024*1024, "L3缓存临界点 (~16MB)")
    ]
    
    for point, label in cache_points:
        # 找到最接近的数据点
        closest_idx = (df['数组大小'] - point).abs().idxmin()
        exact_x = df['数组大小'].iloc[closest_idx]
        if "L3" in label:
            max_y = max(df['两路链式加速比'].max(), df['递归加速比'].max()) * 0.85  # L3标签降低到85%位置
        else:
            max_y = max(df['两路链式加速比'].max(), df['递归加速比'].max()) * 0.95  # 其他保持不变
        
        # 添加垂直线
        plt.axvline(x=exact_x, color='black', linestyle='--', alpha=0.5, linewidth=1.5)
        
        # 添加标签（水平显示）
        if chinese_font is not None:
            plt.text(exact_x - 5, max_y * 1.05, label, 
                    fontproperties=chinese_font, fontsize=10, color='red',
                    bbox=dict(facecolor='white', alpha=0.8, edgecolor='none', pad=2),
                    ha='right', va='bottom')
        else:
            plt.text(exact_x - 5, max_y * 1.05, label, 
                    fontsize=10, color='red',
                    bbox=dict(facecolor='white', alpha=0.8, edgecolor='none', pad=2),
                    ha='right', va='bottom')
    
    # 突出显示性能波动明显的区域
    perf_regions = [
        (60*1024, 70*1024, "L1临界区域", 0.2),
        (950*1024, 1100*1024, "L2临界区域", 0.3),
        (1900*1024, 2100*1024, "L3临界区域", 0.4)
    ]
    
    for x_min, x_max, label, y_pos in perf_regions:
        # 查找区域内的数据
        region_data = df[(df['数组大小'] >= x_min) & (df['数组大小'] <= x_max)]
        if not region_data.empty:
            max_perf = max(df['两路链式加速比'].max(), df['递归加速比'].max())
            y_height = max_perf * y_pos
            plt.fill_between([x_min, x_max], [0, 0], [y_height, y_height], 
                            color='yellow', alpha=0.2, zorder=1)
            
            # 添加区域标签
            mid_x = (x_min + x_max) / 2
            if chinese_font is not None:
                plt.text(mid_x, y_height * 0.5, label,
                        fontproperties=chinese_font, fontsize=9, color='black',
                        ha='center', va='center',
                        bbox=dict(facecolor='white', alpha=0.7, edgecolor='none', pad=2))
            else:
                plt.text(mid_x, y_height * 0.5, label,
                        fontsize=9, color='black',
                        ha='center', va='center',
                        bbox=dict(facecolor='white', alpha=0.7, edgecolor='none', pad=2))
    
    # 在图表上添加缓存大小信息
    if chinese_font is not None:
        plt.figtext(0.01, 0.01, 'CPU缓存: L1=512KB, L2=8MB, L3=16MB', 
                   fontproperties=chinese_font, fontsize=9, alpha=0.7)
    else:
        plt.figtext(0.01, 0.01, 'CPU缓存: L1=512KB, L2=8MB, L3=16MB', 
                   fontsize=9, alpha=0.7)
    
    plt.grid(True, which='both', alpha=0.5)
    
    # 保存图像
    plt.tight_layout()
    plt.savefig('jichu_sum.jpg', dpi=300, bbox_inches='tight')
    print("已保存图像到 jichu_sum.jpg")
    plt.close()

def jinjie_sum(csv_file='jinjie_sum.csv', chinese_font=None):
    """处理进阶求和算法的CSV数据并生成可视化图像 - 针对2的幂次方数据优化"""
    # 读取CSV文件
    df = pd.read_csv(csv_file)
    
    # 获取鲜艳的颜色
    colors = get_vibrant_colors()
    
    # 设置图像大小
    plt.figure(figsize=(12, 7))
    
    # 绘制两种加速比曲线
    plt.plot(df['数组大小'], df['4路展开加速比'], label='4路展开加速比', 
             marker=None, linestyle='-', linewidth=2.5, 
             color=colors['green'], alpha=0.9)
    plt.plot(df['数组大小'], df['8路展开加速比'], label='8路展开加速比', 
             marker=None, linestyle='-', linewidth=2.5, 
             color=colors['purple'], alpha=0.9)
    
    # 识别2的幂次方数值
    power_of_two = []
    power_values = []
    for i in range(7, 26):  # 2^7 到 2^25
        power = 2**i
        power_of_two.append(power)
        power_values.append(i)
    
    # 找出数据中最接近2的幂次方的点
    mark_points_green = []
    mark_points_purple = []
    
    for power in power_of_two:
        # 找到最接近的数据点
        closest_idx = (df['数组大小'] - power).abs().idxmin()
        mark_points_green.append((df['数组大小'].iloc[closest_idx], df['4路展开加速比'].iloc[closest_idx]))
        mark_points_purple.append((df['数组大小'].iloc[closest_idx], df['8路展开加速比'].iloc[closest_idx]))
    
    # 标记2的幂次方点
    x_green = [p[0] for p in mark_points_green]
    y_green = [p[1] for p in mark_points_green]
    x_purple = [p[0] for p in mark_points_purple]
    y_purple = [p[1] for p in mark_points_purple]
    
    plt.plot(x_green, y_green, 'o', markersize=6, color=colors['green'], alpha=0.8)
    plt.plot(x_purple, y_purple, 'x', markersize=7, color=colors['purple'], alpha=0.8)
    
    # 使用中文字体设置标题和标签
    if chinese_font is not None:
        plt.title('进阶求和算法加速比对比', fontproperties=chinese_font, fontsize=18, fontweight='bold', pad=15)
        plt.xlabel('数组大小 (2的幂次方)', fontproperties=chinese_font, fontsize=14, fontweight='bold')
        plt.ylabel('加速比', fontproperties=chinese_font, fontsize=14, fontweight='bold')
        # 设置图例中的中文字体
        legend = plt.legend(fontsize=12, framealpha=0.9, loc='best')
        for text in legend.get_texts():
            text.set_fontproperties(chinese_font)
    else:
        plt.title('进阶求和算法加速比对比', fontsize=18, fontweight='bold', pad=15)
        plt.xlabel('数组大小 (2的幂次方)', fontsize=14, fontweight='bold')
        plt.ylabel('加速比', fontsize=14, fontweight='bold')
        plt.legend(fontsize=12, framealpha=0.9, loc='best')
    
    # 对数刻度设置
    plt.xscale('log', base=2)  # 使用以2为底的对数刻度
    
    # 设置主要刻度为2的幂次方
    major_ticks = [2**i for i in range(7, 26)]
    plt.xticks(major_ticks, ['2^{}'.format(i) for i in range(7, 26)], rotation=45)
    
    # 调整刻度显示 - 根据数据范围显示部分标签
    if len(major_ticks) > 10:
        # 仅保留部分主要刻度标签，避免重叠
        visible_ticks = []
        visible_labels = []
        for i, (tick, label) in enumerate(zip(major_ticks, ['2^{}'.format(i) for i in range(7, 26)])):
            if i % 2 == 0 or i == len(major_ticks) - 1:  # 每隔一个显示，首尾必显示
                visible_ticks.append(tick)
                visible_labels.append(label)
        plt.xticks(visible_ticks, visible_labels, rotation=45)
    
    # 添加缓存临界点垂直线和注释
    cache_points = [
        (64*1024, "L1缓存临界点 (~512KB)"),
        (1024*1024, "L2缓存临界点 (~8MB)"), 
        (2*1024*1024, "L3缓存临界点 (~16MB)")
    ]
    
    for point, label in cache_points:
        # 找到最接近的数据点
        closest_idx = (df['数组大小'] - point).abs().idxmin()
        exact_x = df['数组大小'].iloc[closest_idx]
        # 为 L3 缓存临界点调整 y 坐标位置
        if "L3" in label:
            max_y = max(df['4路展开加速比'].max(), df['8路展开加速比'].max()) * 0.85  # L3标签降低到85%位置
        else:
            max_y = max(df['4路展开加速比'].max(), df['8路展开加速比'].max()) * 0.95  # 其他保持不变
        
        # 添加垂直线
        plt.axvline(x=exact_x, color='black', linestyle='--', alpha=0.5, linewidth=1.5)
        
        # 添加标签（水平显示）
        if chinese_font is not None:
            plt.text(exact_x - 5, max_y * 1.05, label, 
                    fontproperties=chinese_font, fontsize=10, color='red',
                    bbox=dict(facecolor='white', alpha=0.8, edgecolor='none', pad=2),
                    ha='right', va='bottom')
        else:
            plt.text(exact_x - 5, max_y * 1.05, label, 
                    fontsize=10, color='red',
                    bbox=dict(facecolor='white', alpha=0.8, edgecolor='none', pad=2),
                    ha='right', va='bottom')
    
    # 突出显示性能波动明显的区域
    perf_regions = [
        (60*1024, 70*1024, "L1临界区域", 0.2),
        (950*1024, 1100*1024, "L2临界区域", 0.3),
        (1900*1024, 2100*1024, "L3临界区域", 0.4)
    ]
    
    for x_min, x_max, label, y_pos in perf_regions:
        # 查找区域内的数据
        region_data = df[(df['数组大小'] >= x_min) & (df['数组大小'] <= x_max)]
        if not region_data.empty:
            max_perf = max(df['4路展开加速比'].max(), df['8路展开加速比'].max())
            y_height = max_perf * y_pos
            plt.fill_between([x_min, x_max], [0, 0], [y_height, y_height], 
                            color='yellow', alpha=0.2, zorder=1)
            
            # 添加区域标签
            mid_x = (x_min + x_max) / 2
            if chinese_font is not None:
                plt.text(mid_x, y_height * 0.5, label,
                        fontproperties=chinese_font, fontsize=9, color='black',
                        ha='center', va='center',
                        bbox=dict(facecolor='white', alpha=0.7, edgecolor='none', pad=2))
            else:
                plt.text(mid_x, y_height * 0.5, label,
                        fontsize=9, color='black',
                        ha='center', va='center',
                        bbox=dict(facecolor='white', alpha=0.7, edgecolor='none', pad=2))
    
    # 在图表上添加缓存大小信息
    if chinese_font is not None:
        plt.figtext(0.01, 0.01, 'CPU缓存: L1=512KB, L2=8MB, L3=16MB', 
                   fontproperties=chinese_font, fontsize=9, alpha=0.7)
    else:
        plt.figtext(0.01, 0.01, 'CPU缓存: L1=512KB, L2=8MB, L3=16MB', 
                   fontsize=9, alpha=0.7)
    
    plt.grid(True, which='both', alpha=0.5)
    
    # 保存图像
    plt.tight_layout()
    plt.savefig('jinjie_sum.jpg', dpi=300, bbox_inches='tight')
    print("已保存图像到 jinjie_sum.jpg")
    plt.close()

def jichu_matrix(csv_file='jichu_matrix.csv', chinese_font=None):
    """处理基础矩阵算法的CSV数据并生成可视化图像 - 突出缓存临界点"""
    # 读取CSV文件
    df = pd.read_csv(csv_file)
    
    # 获取鲜艳的颜色
    colors = get_vibrant_colors()
    
    # 设置图像大小
    plt.figure(figsize=(14, 8))
    
    # 绘制加速比曲线 - 不使用标记点
    plt.plot(df['矩阵大小'], df['加速比'], label='Cache优化加速比', 
             marker=None, linestyle='-', linewidth=3, 
             color=colors['teal'], alpha=0.9)
    
    # 添加少量均匀分布的数据点标记，而不是每个点
    if len(df) > 30:
        # 选择关键位置的点进行标记
        key_sizes = [10, 50, 100, 200, 250, 500, 800, 1000, 1420, 1700]
        for size in key_sizes:
            # 找到最接近的数据点
            if len(df[df['矩阵大小'] == size]) > 0:
                # 如果精确的size存在
                idx = df[df['矩阵大小'] == size].index[0]
                x = df['矩阵大小'].iloc[idx]
                y = df['加速比'].iloc[idx]
            else:
                # 找最接近的
                closest_idx = (df['矩阵大小'] - size).abs().idxmin()
                x = df['矩阵大小'].iloc[closest_idx]
                y = df['加速比'].iloc[closest_idx]
            
            plt.scatter(x, y, s=100, color=colors['teal'], 
                      edgecolors='white', linewidth=1.5, zorder=3, alpha=0.9)
    else:
        # 数据点较少时保留所有标记
        for x, y in zip(df['矩阵大小'], df['加速比']):
            plt.scatter(x, y, s=80, color=colors['teal'], 
                      edgecolors='white', linewidth=1, zorder=3, alpha=0.8)
    
    # 添加缓存临界点垂直线和注释
    cache_points = [
        (250, "L1缓存临界点 (~512KB)"),
        (1000, "L2缓存临界点 (~8MB)"), 
        (1420, "L3缓存临界点 (~16MB)")
    ]
    
    for point, label in cache_points:
        # 找到最接近的数据点
        closest_idx = (df['矩阵大小'] - point).abs().idxmin()
        exact_x = df['矩阵大小'].iloc[closest_idx]
        max_y = df['加速比'].max() * 0.95  # 放在顶部但留些空间
        
        # 添加垂直线
        plt.axvline(x=exact_x, color='black', linestyle='--', alpha=0.5, linewidth=1.5)
        
        # 添加标签 (水平显示) - 建议修改为以下配置
        if chinese_font is not None:
            plt.text(exact_x - 5, max_y * 1.05, label, 
                    fontproperties=chinese_font, fontsize=10, color='red',
                    bbox=dict(facecolor='white', alpha=0.8, edgecolor='none', pad=2),
                    ha='right', va='bottom')  # 添加va='bottom'确保文本在线上方
        else:
            plt.text(exact_x - 5, max_y * 1.05, label, 
                    fontsize=10, color='red',
                    bbox=dict(facecolor='white', alpha=0.8, edgecolor='none', pad=2),
                    ha='right', va='bottom')
    
    # 突出显示性能波动明显的区域
    perf_regions = [
        (240, 260, "L1临界区域", 0.2),
        (990, 1010, "L2临界区域", 0.3),
        (1400, 1440, "L3临界区域", 0.4)
    ]
    
    for x_min, x_max, label, y_pos in perf_regions:
        # 查找区域内的数据
        region_data = df[(df['矩阵大小'] >= x_min) & (df['矩阵大小'] <= x_max)]
        if not region_data.empty:
            y_height = df['加速比'].max() * y_pos
            plt.fill_between([x_min, x_max], [0, 0], [y_height, y_height], 
                            color='yellow', alpha=0.2, zorder=1)
            
            # 添加区域标签
            mid_x = (x_min + x_max) / 2
            if chinese_font is not None:
                plt.text(mid_x, y_height * 0.5, label,
                        fontproperties=chinese_font, fontsize=9, color='black',
                        ha='center', va='center',
                        bbox=dict(facecolor='white', alpha=0.7, edgecolor='none', pad=2))
            else:
                plt.text(mid_x, y_height * 0.5, label,
                        fontsize=9, color='black',
                        ha='center', va='center',
                        bbox=dict(facecolor='white', alpha=0.7, edgecolor='none', pad=2))
    
    # 使用中文字体设置标题和标签
    if chinese_font is not None:
        plt.title('基础矩阵算法Cache优化加速比与缓存边界关系', fontproperties=chinese_font, fontsize=18, fontweight='bold', pad=15)
        plt.xlabel('矩阵大小', fontproperties=chinese_font, fontsize=14, fontweight='bold')
        plt.ylabel('加速比', fontproperties=chinese_font, fontsize=14, fontweight='bold')
        # 设置图例中的中文字体
        legend = plt.legend(fontsize=12, framealpha=0.9, loc='best')
        for text in legend.get_texts():
            text.set_fontproperties(chinese_font)
    else:
        plt.title('基础矩阵算法Cache优化加速比与缓存边界关系', fontsize=18, fontweight='bold', pad=15)
        plt.xlabel('矩阵大小', fontsize=14, fontweight='bold')
        plt.ylabel('加速比', fontsize=14, fontweight='bold')
        plt.legend(fontsize=12, framealpha=0.9, loc='best')
    
    # 在图表上添加缓存大小信息
    if chinese_font is not None:
        plt.figtext(0.01, 0.01, 'CPU缓存: L1=512KB, L2=8MB, L3=16MB', 
                   fontproperties=chinese_font, fontsize=9, alpha=0.7)
    else:
        plt.figtext(0.01, 0.01, 'CPU缓存: L1=512KB, L2=8MB, L3=16MB', 
                   fontsize=9, alpha=0.7)
    
    plt.grid(True, alpha=0.5)
    
    # 保存图像
    plt.tight_layout()
    plt.savefig('jichu_matrix.jpg', dpi=300, bbox_inches='tight')
    print("已保存图像到 jichu_matrix.jpg")
    plt.close()

def jinjie_matrix(csv_file='jinjie_matrix.csv', chinese_font=None):
    """处理进阶矩阵算法的CSV数据并生成可视化图像 - 突出缓存临界点"""
    # 读取CSV文件
    df = pd.read_csv(csv_file)
    
    # 获取鲜艳的颜色
    colors = get_vibrant_colors()
    
    # 设置图像大小
    plt.figure(figsize=(14, 8))
    
    # 绘制两种加速比曲线 - 不使用标记点
    plt.plot(df['矩阵大小'], df['4路展开加速比'], label='4路展开加速比', 
             marker=None, linestyle='-', linewidth=2.5, 
             color=colors['blue'], alpha=0.9)
    plt.plot(df['矩阵大小'], df['8路展开加速比'], label='8路展开加速比', 
             marker=None, linestyle='-', linewidth=2.5, 
             color=colors['red'], alpha=0.9)
    
    # 添加少量关键位置的数据点标记
    key_sizes = [10, 50, 100, 200, 250, 500, 800, 1000, 1420, 1700]
    for size in key_sizes:
        # 找到最接近的数据点
        if len(df[df['矩阵大小'] == size]) > 0:
            # 如果精确的size存在
            idx = df[df['矩阵大小'] == size].index[0]
            x = df['矩阵大小'].iloc[idx]
            y4 = df['4路展开加速比'].iloc[idx]
            y8 = df['8路展开加速比'].iloc[idx]
        else:
            # 找最接近的
            closest_idx = (df['矩阵大小'] - size).abs().idxmin()
            x = df['矩阵大小'].iloc[closest_idx]
            y4 = df['4路展开加速比'].iloc[closest_idx]
            y8 = df['8路展开加速比'].iloc[closest_idx]
        
        plt.scatter(x, y4, s=80, color=colors['blue'], 
                  edgecolors='white', linewidth=1.5, zorder=3, alpha=0.9)
        plt.scatter(x, y8, s=80, color=colors['red'], 
                  edgecolors='white', linewidth=1.5, zorder=3, alpha=0.9)
    
    # 添加缓存临界点垂直线和注释
    cache_points = [
        (250, "L1缓存临界点 (~512KB)"),
        (1000, "L2缓存临界点 (~8MB)"), 
        (1420, "L3缓存临界点 (~16MB)")
    ]
    
    for point, label in cache_points:
        # 找到最接近的数据点
        closest_idx = (df['矩阵大小'] - point).abs().idxmin()
        exact_x = df['矩阵大小'].iloc[closest_idx]
        max_y = max(df['4路展开加速比'].max(), df['8路展开加速比'].max()) * 0.95  # 放在顶部留些空间
        
        # 添加垂直线
        plt.axvline(x=exact_x, color='black', linestyle='--', alpha=0.5, linewidth=1.5)
        
        # 添加标签 (水平显示) - 建议修改为以下配置
        if chinese_font is not None:
            plt.text(exact_x - 5, max_y * 1.05, label, 
                    fontproperties=chinese_font, fontsize=10, color='red',
                    bbox=dict(facecolor='white', alpha=0.8, edgecolor='none', pad=2),
                    ha='right', va='bottom')  # 添加va='bottom'确保文本在线上方
        else:
            plt.text(exact_x - 5, max_y * 1.05, label, 
                    fontsize=10, color='red',
                    bbox=dict(facecolor='white', alpha=0.8, edgecolor='none', pad=2),
                    ha='right', va='bottom')
    
    # 突出显示性能波动明显的区域
    perf_regions = [
        (240, 260, "L1临界区域\n4路↓,8路↑", 0.2),
        (990, 1010, "L2临界区域", 0.3),
        (1400, 1440, "L3临界区域", 0.4)
    ]
    
    for x_min, x_max, label, y_pos in perf_regions:
        # 查找区域内的数据
        region_data = df[(df['矩阵大小'] >= x_min) & (df['矩阵大小'] <= x_max)]
        if not region_data.empty:
            max_perf = max(df['4路展开加速比'].max(), df['8路展开加速比'].max())
            y_height = max_perf * y_pos
            plt.fill_between([x_min, x_max], [0, 0], [y_height, y_height], 
                            color='yellow', alpha=0.2, zorder=1)
            
            # 添加区域标签
            mid_x = (x_min + x_max) / 2
            if chinese_font is not None:
                plt.text(mid_x, y_height * 0.5, label,
                        fontproperties=chinese_font, fontsize=9, color='black',
                        ha='center', va='center',
                        bbox=dict(facecolor='white', alpha=0.7, edgecolor='none', pad=2))
            else:
                plt.text(mid_x, y_height * 0.5, label,
                        fontsize=9, color='black',
                        ha='center', va='center',
                        bbox=dict(facecolor='white', alpha=0.7, edgecolor='none', pad=2))
    
    # 使用中文字体设置标题和标签
    if chinese_font is not None:
        plt.title('进阶矩阵算法加速比对比与缓存边界关系', fontproperties=chinese_font, fontsize=18, fontweight='bold', pad=15)
        plt.xlabel('矩阵大小', fontproperties=chinese_font, fontsize=14, fontweight='bold')
        plt.ylabel('加速比', fontproperties=chinese_font, fontsize=14, fontweight='bold')
        # 设置图例中的中文字体
        legend = plt.legend(fontsize=12, framealpha=0.9, loc='best')
        for text in legend.get_texts():
            text.set_fontproperties(chinese_font)
    else:
        plt.title('进阶矩阵算法加速比对比与缓存边界关系', fontsize=18, fontweight='bold', pad=15)
        plt.xlabel('矩阵大小', fontsize=14, fontweight='bold')
        plt.ylabel('加速比', fontsize=14, fontweight='bold')
        plt.legend(fontsize=12, framealpha=0.9, loc='best')
    
    # 在图表上添加缓存大小信息
    if chinese_font is not None:
        plt.figtext(0.01, 0.01, 'CPU缓存: L1=512KB, L2=8MB, L3=16MB', 
                   fontproperties=chinese_font, fontsize=9, alpha=0.7)
    else:
        plt.figtext(0.01, 0.01, 'CPU缓存: L1=512KB, L2=8MB, L3=16MB', 
                   fontsize=9, alpha=0.7)
    
    plt.grid(True, alpha=0.5)
    
    # 保存图像
    plt.tight_layout()
    plt.savefig('jinjie_matrix.jpg', dpi=300, bbox_inches='tight')
    print("已保存图像到 jinjie_matrix.jpg")
    plt.close()
    
def process_all_csv():
    """处理所有4个CSV文件并生成对应的可视化图像"""
    # 设置图表样式和获取中文字体
    chinese_font = setup_plot_style()
    
    print("使用中文字体:", chinese_font.get_name() if hasattr(chinese_font, 'get_name') else "系统默认中文字体")
    
    # 检查CSV文件是否存在，并处理
    csv_files = ['jichu_sum.csv', 'jinjie_sum.csv', 'jichu_matrix.csv', 'jinjie_matrix.csv']
    process_functions = [jichu_sum, jinjie_sum, jichu_matrix, jinjie_matrix]
    
    for csv_file, process_func in zip(csv_files, process_functions):
        if os.path.exists(csv_file):
            print("处理文件:", csv_file)
            process_func(csv_file, chinese_font)
        else:
            print("文件不存在:", csv_file)

if __name__ == "__main__":
    print("开始生成可视化图像...")
    process_all_csv()
    print("所有图像生成完成")