"""
小企业自动化管理后台（Streamlit）
功能：订单概览、报表下载、任务监控、消息推送
启动: streamlit run admin_app.py
"""

import streamlit as st
import pandas as pd
from datetime import datetime, timedelta
from pathlib import Path
import sys

# 添加工具模块路径
sys.path.insert(0, str(Path(__file__).parent.parent))
from 消息推送.wecom_bot import WeComBot

# ============================================================
# 页面配置
# ============================================================

st.set_page_config(
    page_title="小企业自动化后台",
    page_icon="📊",
    layout="wide",
)

# ============================================================
# 登录（简易版，生产环境换数据库）
# ============================================================

if "logged_in" not in st.session_state:
    st.session_state.logged_in = False

if not st.session_state.logged_in:
    st.title("小企业自动化后台")
    with st.form("login"):
        user = st.text_input("用户名")
        pwd = st.text_input("密码", type="password")
        if st.form_submit_button("登录"):
            if user == "admin" and pwd == "auto2024":
                st.session_state.logged_in = True
                st.rerun()
            else:
                st.error("用户名或密码错误")
    st.stop()

# ============================================================
# 侧边栏导航
# ============================================================

st.sidebar.title("📊 自动化后台")
menu = st.sidebar.radio("导航", ["仪表盘", "订单管理", "报表下载", "消息推送", "定时任务", "系统设置"])

# 退出按钮
if st.sidebar.button("退出登录"):
    st.session_state.logged_in = False
    st.rerun()

st.sidebar.caption(f"当前时间：{datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")


# ============================================================
# 仪表盘
# ============================================================

if menu == "仪表盘":
    st.title("运营仪表盘")

    col1, col2, col3, col4 = st.columns(4)
    col1.metric("今日订单", "128", "+12%")
    col2.metric("今日销售额", "¥18,520", "+8%")
    col3.metric("待发货", "15", "-3")
    col4.metric("退款率", "2.1%", "-0.5%")

    st.divider()

    # 上传数据文件看图表
    uploaded = st.file_uploader("上传订单 CSV 查看图表", type=["csv"])
    if uploaded:
        df = pd.read_csv(uploaded)
        st.subheader("近 7 天销售趋势")
        if "日期" in df.columns:
            df["日期"] = pd.to_datetime(df["日期"])
            daily = df.groupby(df["日期"].dt.date)["金额"].sum()
            st.line_chart(daily)

        col1, col2 = st.columns(2)
        with col1:
            if "商品" in df.columns:
                st.subheader("商品销量排行")
                product_top = df.groupby("商品")["金额"].sum().nlargest(10)
                st.bar_chart(product_top)
        with col2:
            if "地区" in df.columns:
                st.subheader("地区分布")
                region = df["地区"].value_counts()
                st.bar_chart(region)


# ============================================================
# 订单管理
# ============================================================

elif menu == "订单管理":
    st.title("订单管理")

    tab1, tab2 = st.tabs(["订单列表", "批量操作"])

    with tab1:
        uploaded = st.file_uploader("导入订单 CSV", type=["csv"], key="order_csv")
        if uploaded:
            df = pd.read_csv(uploaded)
            search = st.text_input("搜索订单号 / 收货人")
            if search:
                mask = df.astype(str).apply(lambda row: row.str.contains(search).any(), axis=1)
                df = df[mask]
            st.dataframe(df, use_container_width=True, height=400)

    with tab2:
        st.subheader("批量生成发货单")
        batch_file = st.file_uploader("选择订单文件", type=["csv"], key="batch_csv")
        if batch_file and st.button("生成发货单"):
            from 报表生成.report_generator import generate_delivery_notes
            temp_path = Path("E:/项目/小企业自动化/temp_orders.csv")
            temp_path.write_bytes(batch_file.read())
            result = generate_delivery_notes(str(temp_path))
            st.success(f"已生成 {len(result)} 份发货单")


# ============================================================
# 报表下载
# ============================================================

elif menu == "报表下载":
    st.title("报表中心")

    col1, col2, col3 = st.columns(3)
    with col1:
        if st.button("生成今日日报", use_container_width=True):
            st.info("日报将包含：订单明细 + 商品汇总 + 地区汇总")
    with col2:
        if st.button("生成本周周报", use_container_width=True):
            st.info("周报将包含：7天销售趋势 + 对比上周")
    with col3:
        if st.button("生成本月月报", use_container_width=True):
            st.info("月报将包含：月度汇总 + 利润估算 + 趋势分析")

    st.divider()
    st.subheader("历史报表")
    report_dir = Path("E:/报表/")
    if report_dir.exists():
        for f in sorted(report_dir.glob("*.xlsx"), reverse=True):
            st.write(f"📄 {f.name} — {datetime.fromtimestamp(f.stat().st_mtime).strftime('%Y-%m-%d %H:%M')}")


# ============================================================
# 消息推送
# ============================================================

elif menu == "消息推送":
    st.title("消息推送")

    with st.form("wecom_config"):
        st.subheader("企业微信机器人配置")
        webhook = st.text_input("Webhook URL", type="password")
        test_msg = st.text_input("测试消息内容")
        if st.form_submit_button("发送测试"):
            if webhook:
                bot = WeComBot(webhook)
                result = bot.send_text(test_msg or "✅ 自动化后台测试消息")
                st.json(result)

    st.divider()

    with st.form("push_schedule"):
        st.subheader("定时推送设置")
        st.selectbox("推送类型", ["每日运营日报", "异常告警", "发货提醒", "库存预警"])
        st.time_input("推送时间", value=datetime.strptime("09:00", "%H:%M").time())
        at_users = st.text_input("@提醒谁（手机号，逗号分隔）")
        if st.form_submit_button("保存设置"):
            st.success("推送计划已保存")


# ============================================================
# 定时任务
# ============================================================

elif menu == "定时任务":
    st.title("定时任务监控")

    tasks = [
        {"名称": "订单自动抓取", "状态": "运行中", "频率": "每30分钟", "上次执行": "10分钟前", "结果": "成功"},
        {"名称": "每日报表生成", "状态": "等待中", "频率": "每天08:00", "上次执行": "昨天08:00", "结果": "成功"},
        {"名称": "发货单批量生成", "状态": "已暂停", "频率": "手动触发", "上次执行": "2小时前", "结果": "部分失败"},
    ]

    for t in tasks:
        color = "green" if t["状态"] == "运行中" else ("orange" if t["状态"] == "等待中" else "red")
        st.markdown(
            f"| :{color}[●] {t['名称']} | {t['频率']} | {t['上次执行']} | {t['结果']} |"
        )

    st.divider()
    if st.button("手动触发：订单抓取"):
        st.info("已加入执行队列，预计 2 分钟后完成")


# ============================================================
# 系统设置
# ============================================================

elif menu == "系统设置":
    st.title("系统设置")

    with st.expander("企业微信通知", expanded=True):
        st.text_input("Webhook URL", value="https://qyapi.weixin.qq.com/...", type="password")
        st.button("保存并测试")

    with st.expander("钉钉通知"):
        st.text_input("Webhook URL", type="password")
        st.text_input("加签密钥", type="password")
        st.button("保存并测试", key="dingtalk_save")

    with st.expander("数据存储"):
        st.text_input("订单数据路径", value="E:/项目/小企业自动化/data/")
        st.text_input("报表输出路径", value="E:/报表/")
        st.button("保存路径设置")

    with st.expander("管理员账号"):
        st.text_input("新密码", type="password")
        st.button("修改密码")
