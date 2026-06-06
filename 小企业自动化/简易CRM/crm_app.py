"""
简易 CRM 系统（Streamlit）
功能：客户管理、跟进记录、到期提醒
启动: streamlit run crm_app.py
"""

import streamlit as st
import pandas as pd
from datetime import datetime, date, timedelta
from pathlib import Path
import json

DATA_DIR = Path("E:/项目/小企业自动化/crm_data")
DATA_DIR.mkdir(parents=True, exist_ok=True)
CUSTOMERS_FILE = DATA_DIR / "customers.json"
FOLLOWUPS_FILE = DATA_DIR / "followups.json"


# ============================================================
# 数据持久化（简单 JSON，生产环境换 SQLite）
# ============================================================

def load_json(path: Path, default: list) -> list:
    if path.exists():
        return json.loads(path.read_text(encoding="utf-8"))
    return default


def save_json(path: Path, data: list):
    path.write_text(json.dumps(data, ensure_ascii=False, indent=2), encoding="utf-8")


# ============================================================
# 页面配置
# ============================================================

st.set_page_config(page_title="简易 CRM", page_icon="👥", layout="wide")
st.title("👥 客户管理系统")

# ============================================================
# 侧边栏：快速操作
# ============================================================

with st.sidebar:
    st.subheader("快捷操作")
    if st.button("📋 今日需要跟进", use_container_width=True):
        st.session_state.filter = "today"
    if st.button("⚠️ 逾期未跟进", use_container_width=True):
        st.session_state.filter = "overdue"
    if st.button("📊 导出客户列表", use_container_width=True):
        st.session_state.filter = "all"

    st.divider()
    st.caption(f"数据目录：{DATA_DIR}")

# ============================================================
# Tab: 客户列表
# ============================================================

tab1, tab2, tab3 = st.tabs(["客户列表", "跟进记录", "提醒设置"])

with tab1:
    customers = load_json(CUSTOMERS_FILE, [])

    # 新建客户表单
    with st.expander("+ 新建客户", expanded=False):
        with st.form("new_customer"):
            col1, col2, col3 = st.columns(3)
            name = col1.text_input("客户名称 *")
            phone = col2.text_input("手机号")
            company = col3.text_input("公司")
            col1, col2, col3 = st.columns(3)
            source = col1.selectbox("来源", ["朋友介绍", "线上咨询", "老客户介绍", "闲鱼", "小红书", "其他"])
            status = col2.selectbox("状态", ["新客户", "已报价", "已成交", "已流失", "意向中"])
            tags = col3.text_input("标签（逗号分隔）", placeholder="电商,广州,批发")
            note = st.text_area("备注")
            next_followup = st.date_input("下次跟进日期", value=date.today() + timedelta(days=3))
            if st.form_submit_button("保存客户"):
                customers.append({
                    "id": datetime.now().strftime("%Y%m%d%H%M%S"),
                    "名称": name,
                    "手机": phone,
                    "公司": company,
                    "来源": source,
                    "状态": status,
                    "标签": tags,
                    "备注": note,
                    "下次跟进": str(next_followup),
                    "创建时间": datetime.now().strftime("%Y-%m-%d %H:%M"),
                })
                save_json(CUSTOMERS_FILE, customers)
                st.success(f"客户 {name} 已保存")
                st.rerun()

    # 客户搜索和筛选
    col1, col2, col3 = st.columns(3)
    search = col1.text_input("🔍 搜索客户", placeholder="名称 / 手机 / 公司")
    status_filter = col2.selectbox("状态筛选", ["全部", "新客户", "已报价", "已成交", "已流失", "意向中"])
    tag_filter = col3.text_input("标签筛选", placeholder="电商 / 广州")

    # 应用筛选
    df = pd.DataFrame(customers) if customers else pd.DataFrame()
    if not df.empty:
        if search:
            mask = df.astype(str).apply(lambda r: r.str.contains(search, case=False).any(), axis=1)
            df = df[mask]
        if status_filter != "全部":
            df = df[df["状态"] == status_filter]
        if tag_filter:
            df = df[df["标签"].str.contains(tag_filter, case=False, na=False)]

        # 高亮逾期跟进
        def highlight_overdue(row):
            try:
                followup = datetime.strptime(str(row["下次跟进"]), "%Y-%m-%d").date()
                if followup < date.today():
                    return ["background-color: #fff0f0"] * len(row)
            except:
                pass
            return [""] * len(row)

        st.dataframe(
            df.style.apply(highlight_overdue, axis=1),
            use_container_width=True,
            height=400,
            column_config={
                "id": None,
            },
        )

        # 操作按钮
        if st.button("删除选中客户"):
            pass  # 生产环境需选择行后删除

    else:
        st.info("暂无客户数据，点击上方「新建客户」开始")

with tab2:
    st.subheader("跟进记录")
    followups = load_json(FOLLOWUPS_FILE, [])

    with st.form("new_followup"):
        col1, col2 = st.columns(2)
        customer_name = col1.text_input("客户名称 *")
        method = col2.selectbox("跟进方式", ["电话", "微信", "面谈", "邮件", "其他"])
        content = st.text_area("跟进内容 *")
        next_date = st.date_input("下次跟进", value=date.today() + timedelta(days=7))
        if st.form_submit_button("保存跟进记录"):
            followups.append({
                "客户": customer_name,
                "方式": method,
                "内容": content,
                "下次跟进": str(next_date),
                "时间": datetime.now().strftime("%Y-%m-%d %H:%M"),
            })
            save_json(FOLLOWUPS_FILE, followups)

            # 同时更新客户表的下次跟进日期
            for c in customers:
                if c["名称"] == customer_name:
                    c["下次跟进"] = str(next_date)
                    save_json(CUSTOMERS_FILE, customers)
                    break

            st.success("跟进记录已保存")
            st.rerun()

    if followups:
        st.dataframe(
            pd.DataFrame(followups).sort_values("时间", ascending=False),
            use_container_width=True,
            height=300,
        )

with tab3:
    st.subheader("今日跟进提醒")

    if customers:
        today = str(date.today())
        upcoming = []
        overdue = []

        for c in customers:
            followup = c.get("下次跟进", "")
            if followup == today:
                upcoming.append(c)
            elif followup and followup < today:
                overdue.append(c)

        if upcoming:
            st.success(f"📋 今日需跟进：{len(upcoming)} 位客户")
            for c in upcoming:
                st.write(f"- **{c['名称']}** | {c.get('手机', '')} | {c.get('状态', '')} | 备注：{c.get('备注', '')}")

        if overdue:
            st.error(f"⚠️ 逾期未跟进：{len(overdue)} 位客户")
            for c in overdue:
                days = (date.today() - datetime.strptime(c["下次跟进"], "%Y-%m-%d").date()).days
                st.write(f"- **{c['名称']}** | 逾期 {days} 天 | 手机：{c.get('手机', '')} | 状态：{c.get('状态', '')}")
        if not upcoming and not overdue:
            st.info("今天没有需要跟进的客户 ✓")
    else:
        st.info("暂无客户数据")
