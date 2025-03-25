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
    """处理基础求和算法的CSV数据并生成可视化图像"""
    # 读取CSV文件
    df = pd.read_csv(csv_file)
    
    # 获取鲜艳的颜色
    colors = get_vibrant_colors()
    
    # 设置图像大小
    plt.figure(figsize=(12, 7))
    
    # 绘制两种加速比曲线
    plt.plot(df['数组大小'], df['两路链式加速比'], label='两路链式加速比', 
             marker='o', markersize=5, linestyle='-', linewidth=2.5, 
             color=colors['blue'], alpha=0.9)
    plt.plot(df['数组大小'], df['递归加速比'], label='递归加速比', 
             marker='x', markersize=6, linestyle='-', linewidth=2.5, 
             color=colors['red'], alpha=0.9)
    
    # 使用中文字体设置标题和标签
    if chinese_font is not None:
        plt.title('基础求和算法加速比对比', fontproperties=chinese_font, fontsize=18, fontweight='bold', pad=15)
        plt.xlabel('数组大小', fontproperties=chinese_font, fontsize=14, fontweight='bold')
        plt.ylabel('加速比', fontproperties=chinese_font, fontsize=14, fontweight='bold')
        # 设置图例中的中文字体
        legend = plt.legend(fontsize=12, framealpha=0.9, loc='best')
        for text in legend.get_texts():
            text.set_fontproperties(chinese_font)
    else:
        plt.title('基础求和算法加速比对比', fontsize=18, fontweight='bold', pad=15)
        plt.xlabel('数组大小', fontsize=14, fontweight='bold')
        plt.ylabel('加速比', fontsize=14, fontweight='bold')
        plt.legend(fontsize=12, framealpha=0.9, loc='best')
    
    plt.grid(True, alpha=0.5)
    plt.xscale('log')  # 使用对数刻度
    
    # 保存图像
    plt.tight_layout()
    plt.savefig('jichu_sum.jpg', dpi=300, bbox_inches='tight')
    print("已保存图像到 jichu_sum.jpg")
    plt.close()

def jinjie_sum(csv_file='jinjie_sum.csv', chinese_font=None):
    """处理进阶求和算法的CSV数据并生成可视化图像"""
    # 读取CSV文件
    df = pd.read_csv(csv_file)
    
    # 获取鲜艳的颜色
    colors = get_vibrant_colors()
    
    # 设置图像大小
    plt.figure(figsize=(12, 7))
    
    # 绘制两种加速比曲线
    plt.plot(df['数组大小'], df['4路展开加速比'], label='4路展开加速比', 
             marker='o', markersize=5, linestyle='-', linewidth=2.5, 
             color=colors['green'], alpha=0.9)
    plt.plot(df['数组大小'], df['8路展开加速比'], label='8路展开加速比', 
             marker='x', markersize=6, linestyle='-', linewidth=2.5, 
             color=colors['purple'], alpha=0.9)
    
    # 使用中文字体设置标题和标签
    if chinese_font is not None:
        plt.title('进阶求和算法加速比对比', fontproperties=chinese_font, fontsize=18, fontweight='bold', pad=15)
        plt.xlabel('数组大小', fontproperties=chinese_font, fontsize=14, fontweight='bold')
        plt.ylabel('加速比', fontproperties=chinese_font, fontsize=14, fontweight='bold')
        # 设置图例中的中文字体
        legend = plt.legend(fontsize=12, framealpha=0.9, loc='best')
        for text in legend.get_texts():
            text.set_fontproperties(chinese_font)
    else:
        plt.title('进阶求和算法加速比对比', fontsize=18, fontweight='bold', pad=15)
        plt.xlabel('数组大小', fontsize=14, fontweight='bold')
        plt.ylabel('加速比', fontsize=14, fontweight='bold')
        plt.legend(fontsize=12, framealpha=0.9, loc='best')
    
    plt.grid(True, alpha=0.5)
    plt.xscale('log')  # 使用对数刻度
    
    # 保存图像
    plt.tight_layout()
    plt.savefig('jinjie_sum.jpg', dpi=300, bbox_inches='tight')
    print("已保存图像到 jinjie_sum.jpg")
    plt.close()

def jichu_matrix(csv_file='jichu_matrix.csv', chinese_font=None):
    """处理基础矩阵算法的CSV数据并生成可视化图像"""
    # 读取CSV文件
    df = pd.read_csv(csv_file)
    
    # 获取鲜艳的颜色
    colors = get_vibrant_colors()
    
    # 设置图像大小
    plt.figure(figsize=(12, 7))
    
    # 绘制加速比曲线
    plt.plot(df['矩阵大小'], df['加速比'], label='Cache优化加速比', 
             marker='o', markersize=6, linestyle='-', linewidth=3, 
             color=colors['teal'], alpha=0.9)
    
    # 添加数据点
    for x, y in zip(df['矩阵大小'], df['加速比']):
        plt.scatter(x, y, s=80, color=colors['teal'], edgecolors='white', linewidth=1, zorder=3, alpha=0.8)
    
    # 使用中文字体设置标题和标签
    if chinese_font is not None:
        plt.title('基础矩阵算法Cache优化加速比', fontproperties=chinese_font, fontsize=18, fontweight='bold', pad=15)
        plt.xlabel('矩阵大小', fontproperties=chinese_font, fontsize=14, fontweight='bold')
        plt.ylabel('加速比', fontproperties=chinese_font, fontsize=14, fontweight='bold')
        # 设置图例中的中文字体
        legend = plt.legend(fontsize=12, framealpha=0.9, loc='best')
        for text in legend.get_texts():
            text.set_fontproperties(chinese_font)
    else:
        plt.title('基础矩阵算法Cache优化加速比', fontsize=18, fontweight='bold', pad=15)
        plt.xlabel('矩阵大小', fontsize=14, fontweight='bold')
        plt.ylabel('加速比', fontsize=14, fontweight='bold')
        plt.legend(fontsize=12, framealpha=0.9, loc='best')
    
    plt.grid(True, alpha=0.5)
    
    # 保存图像
    plt.tight_layout()
    plt.savefig('jichu_matrix.jpg', dpi=300, bbox_inches='tight')
    print("已保存图像到 jichu_matrix.jpg")
    plt.close()

def jinjie_matrix(csv_file='jinjie_matrix.csv', chinese_font=None):
    """处理进阶矩阵算法的CSV数据并生成可视化图像"""
    # 读取CSV文件
    df = pd.read_csv(csv_file)
    
    # 获取鲜艳的颜色
    colors = get_vibrant_colors()
    
    # 设置图像大小
    plt.figure(figsize=(12, 7))
    
    # 绘制两种加速比曲线 - 修改颜色组合以增强区分度
    plt.plot(df['矩阵大小'], df['4路展开加速比'], label='4路展开加速比', 
             marker='o', markersize=5, linestyle='-', linewidth=2.5, 
             color=colors['blue'], alpha=0.9)  # 改为蓝色
    plt.plot(df['矩阵大小'], df['8路展开加速比'], label='8路展开加速比', 
             marker='x', markersize=6, linestyle='-', linewidth=2.5, 
             color=colors['red'], alpha=0.9)   # 改为红色
    
    # 使用中文字体设置标题和标签
    if chinese_font is not None:
        plt.title('进阶矩阵算法加速比对比', fontproperties=chinese_font, fontsize=18, fontweight='bold', pad=15)
        plt.xlabel('矩阵大小', fontproperties=chinese_font, fontsize=14, fontweight='bold')
        plt.ylabel('加速比', fontproperties=chinese_font, fontsize=14, fontweight='bold')
        # 设置图例中的中文字体
        legend = plt.legend(fontsize=12, framealpha=0.9, loc='best')
        for text in legend.get_texts():
            text.set_fontproperties(chinese_font)
    else:
        plt.title('进阶矩阵算法加速比对比', fontsize=18, fontweight='bold', pad=15)
        plt.xlabel('矩阵大小', fontsize=14, fontweight='bold')
        plt.ylabel('加速比', fontsize=14, fontweight='bold')
        plt.legend(fontsize=12, framealpha=0.9, loc='best')
    
    plt.grid(True, alpha=0.5)
    plt.xscale('log')  # 使用对数刻度
    
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