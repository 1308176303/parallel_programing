import pandas as pd
import matplotlib.pyplot as plt
from scipy.interpolate import make_interp_spline
import numpy as np


def set_plot_style():
    plt.style.use('seaborn-v0_8-white')  # 使用白色主题
    plt.rcParams.update({
        'font.family': ['SimHei', 'DejaVu Sans'],
        'axes.unicode_minus': False,
        'mathtext.fontset': 'cm',
        'axes.facecolor': '#f0f0f0',  # 浅灰色背景
        'axes.grid': True,
        'grid.color': 'white',  # 白色网格
        'grid.linewidth': 1.5,
        'axes.spines.top': False,  # 移除上边框
        'axes.spines.right': False,  # 移除右边框
        'figure.facecolor': 'white',
        'axes.labelcolor': '#2f2f2f',
        'xtick.color': '#2f2f2f',
        'ytick.color': '#2f2f2f',
    })


def smooth_curve(x, y):
    x_new = np.linspace(min(x), max(x), 300)
    spl = make_interp_spline(x, y, k=3)
    y_new = spl(x_new)
    return x_new, y_new


def jichu_matrix():
    df = pd.read_csv('jichu_matrix.csv')
    df = df[df['矩阵大小'] <= 5000]

    plt.figure(figsize=(12, 8))
    set_plot_style()

    x = df['矩阵大小']
    colors = ['#2C3E50', '#E74C3C']  # 深蓝和红色
    for (col, color, label) in [('平凡算法(秒)', colors[0], '平凡算法'),
                               ('Cache优化(秒)', colors[1], 'Cache优化')]:
        x_smooth, y_smooth = smooth_curve(x, df[col])
        plt.plot(x_smooth, y_smooth, color=color, linewidth=3, label=label)
        plt.scatter(x, df[col], color=color, s=60, alpha=0.6, edgecolor='white')

    plt.title('基础矩阵乘法性能对比', fontsize=16, pad=20)
    plt.xlabel('矩阵大小', fontsize=14)
    plt.ylabel('计算时间 (秒)', fontsize=14)
    plt.legend(fontsize=12, framealpha=0.8)
    plt.yscale('log')
    plt.tight_layout()
    plt.savefig('jichu_matrix.jpg', dpi=300, bbox_inches='tight')
    plt.close()


def jichu_sum():
    df = pd.read_csv('jichu_sum.csv')
    plt.figure(figsize=(12, 8))
    set_plot_style()

    colors = ['#2C3E50', '#E74C3C', '#27AE60']  # 深蓝、红色和绿色
    for (col, color, label) in [('平凡算法(秒)', colors[0], '平凡算法'),
                               ('两路链式(秒)', colors[1], '两路链式'),
                               ('递归(秒)', colors[2], '递归')]:
        valid_data = df[df[col].notna()]
        x_smooth, y_smooth = smooth_curve(valid_data['数组大小'], valid_data[col])
        plt.plot(x_smooth, y_smooth, color=color, linewidth=3, label=label)
        plt.scatter(valid_data['数组大小'], valid_data[col], color=color, s=60, alpha=0.6, edgecolor='white')

    plt.title('基础求和算法性能对比', fontsize=16, pad=20)
    plt.xlabel('数组大小', fontsize=14)
    plt.ylabel('计算时间 (秒)', fontsize=14)
    plt.legend(fontsize=12, framealpha=0.8)
    plt.yscale('log')
    plt.xscale('log')
    plt.tight_layout()
    plt.savefig('jichu_sum.jpg', dpi=300, bbox_inches='tight')
    plt.close()


def jinjie_matrix():
    df = pd.read_csv('jinjie_matrix.csv')
    plt.figure(figsize=(12, 8))
    set_plot_style()

    colors = ['#2C3E50', '#E74C3C', '#27AE60']
    for (col, color, label) in [('平凡算法(秒)', colors[0], '平凡算法'),
                               ('4路展开(秒)', colors[1], '4路展开'),
                               ('8路展开(秒)', colors[2], '8路展开')]:
        x_smooth, y_smooth = smooth_curve(df['矩阵大小'], df[col])
        plt.plot(x_smooth, y_smooth, color=color, linewidth=3, label=label)
        plt.scatter(df['矩阵大小'], df[col], color=color, s=60, alpha=0.6, edgecolor='white')

    plt.title('进阶矩阵乘法性能对比', fontsize=16, pad=20)
    plt.xlabel('矩阵大小', fontsize=14)
    plt.ylabel('计算时间 (秒)', fontsize=14)
    plt.legend(fontsize=12, framealpha=0.8)
    plt.yscale('log')
    plt.tight_layout()
    plt.savefig('jinjie_matrix.jpg', dpi=300, bbox_inches='tight')
    plt.close()


def jinjie_sum():
    df = pd.read_csv('jinjie_sum.csv')
    plt.figure(figsize=(12, 8))
    set_plot_style()

    colors = ['#2C3E50', '#E74C3C', '#27AE60']
    for (col, color, label) in [('平凡算法(秒)', colors[0], '平凡算法'),
                               ('4路展开(秒)', colors[1], '4路展开'),
                               ('8路展开(秒)', colors[2], '8路展开')]:
        x_smooth, y_smooth = smooth_curve(df['数组大小'], df[col])
        plt.plot(x_smooth, y_smooth, color=color, linewidth=3, label=label)
        plt.scatter(df['数组大小'], df[col], color=color, s=60, alpha=0.6, edgecolor='white')

    plt.title('进阶求和算法性能对比', fontsize=16, pad=20)
    plt.xlabel('数组大小', fontsize=14)
    plt.ylabel('计算时间 (秒)', fontsize=14)
    plt.legend(fontsize=12, framealpha=0.8)
    plt.yscale('log')
    plt.xscale('log')
    plt.tight_layout()
    plt.savefig('jinjie_sum.jpg', dpi=300, bbox_inches='tight')
    plt.close()


# Generate all plots
jichu_matrix()
jichu_sum()
jinjie_matrix()
jinjie_sum()



