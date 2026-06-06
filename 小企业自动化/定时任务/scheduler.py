"""
定时任务调度器
Windows：用任务计划程序 + 此脚本
Linux：用 cron + 此脚本
也可以用 schedule 库在 Python 进程内跑
"""

import subprocess
import sys
import json
from pathlib import Path
from datetime import datetime

TASK_CONFIG = {
    "订单抓取": {
        "script": "数据抓取/order_scraper.py",
        "schedule": "*/30 * * * *",  # 每 30 分钟
    },
    "每日报表": {
        "script": "报表生成/report_generator.py",
        "schedule": "0 8 * * *",  # 每天 8:00
        "args": "daily",
    },
    "消息推送": {
        "script": "消息推送/wecom_bot.py",
        "schedule": "0 9 * * *",  # 每天 9:00
    },
}


# ============================================================
# 方案一：Python 内置调度（最简单）
# ============================================================

def run_scheduler():
    """使用 schedule 库常驻进程运行"""
    import schedule
    import time

    base = Path(__file__).parent.parent

    for name, cfg in TASK_CONFIG.items():
        def make_job(script=cfg["script"], args=cfg.get("args", "")):
            def job():
                script_path = str(base / script)
                print(f"[{datetime.now()}] 执行任务: {name}")
                try:
                    result = subprocess.run(
                        [sys.executable, script_path] + (args.split() if args else []),
                        capture_output=True, text=True, timeout=300, cwd=str(base),
                    )
                    print(result.stdout)
                    if result.returncode != 0:
                        print(f"错误: {result.stderr}")
                except Exception as e:
                    print(f"任务失败: {e}")
            return job

        # 解析 cron 表达式简化版：只支持 *, */N, N 格式
        parts = cfg["schedule"].split()
        minute, hour = parts[0], parts[1]

        if minute.startswith("*/"):
            interval = int(minute[2:])
            schedule.every(interval).minutes.do(make_job())
        elif hour != "*":
            schedule.every().day.at(f"{hour.zfill(2)}:{minute.zfill(2)}").do(make_job())
        else:
            schedule.every().hour.do(make_job())

    print("调度器已启动，按 Ctrl+C 停止")
    while True:
        schedule.run_pending()
        time.sleep(30)


# ============================================================
# 方案二：Windows 任务计划（生产推荐）
# ============================================================

def install_windows_task(task_name: str, script_path: str, interval_minutes: int = 30):
    """创建一个 Windows 计划任务"""
    python = sys.executable
    folder = str(Path(script_path).parent)

    cmd = (
        f'schtasks /Create /SC MINUTE /MO {interval_minutes} '
        f'/TN "{task_name}" /TR "\\"{python}\\" \\"{script_path}\\"" '
        f'/ST 00:00 /F'
    )
    result = subprocess.run(cmd, shell=True, capture_output=True, text=True)
    if result.returncode == 0:
        print(f"Windows 计划任务已创建: {task_name}")
    else:
        print(f"创建失败: {result.stderr}")


def list_windows_tasks():
    """列出所有小企业自动化相关的计划任务"""
    subprocess.run('schtasks /Query /FO LIST | findstr /C:"TaskName" /C:"Next Run"', shell=True)


def remove_windows_task(task_name: str):
    """删除计划任务"""
    subprocess.run(f'schtasks /Delete /TN "{task_name}" /F', shell=True)


# ============================================================
# 方案三：Linux / Mac Cron
# ============================================================

def generate_crontab(base_dir: str) -> str:
    """生成 crontab 配置, 用户手动添加到 crontab -e"""
    lines = ["# 小企业自动化定时任务"]
    python = sys.executable

    for name, cfg in TASK_CONFIG.items():
        script = str(Path(base_dir) / cfg["script"])
        cron = cfg["schedule"]
        args = cfg.get("args", "")
        lines.append(f"{cron} {python} {script} {args} # {name}")

    return "\n".join(lines)


# ============================================================
# CLI 入口
# ============================================================

if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(description="定时任务管理")
    parser.add_argument("action", choices=["start", "install", "list", "remove", "cron"],
                        help="start=常驻进程 | install=Windows计划任务 | list=查看 | remove=删除 | cron=输出crontab")
    parser.add_argument("--name", default="小企业自动化", help="任务名称")
    parser.add_argument("--interval", type=int, default=30, help="间隔分钟数")
    args = parser.parse_args()

    base = Path(__file__).parent.parent

    if args.action == "start":
        run_scheduler()
    elif args.action == "install":
        install_windows_task(args.name, str(base / TASK_CONFIG["订单抓取"]["script"]), args.interval)
    elif args.action == "list":
        list_windows_tasks()
    elif args.action == "remove":
        remove_windows_task(args.name)
    elif args.action == "cron":
        print(generate_crontab(str(base)))
