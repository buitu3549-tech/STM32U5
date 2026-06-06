"""
电商订单数据抓取模板
支持：模拟浏览器登录 + API 对接两种方式
依赖：playwright, requests, pandas
"""

import asyncio
import time
from pathlib import Path
import pandas as pd
import requests
from playwright.async_api import async_playwright


# ============================================================
# 方案一：模拟浏览器抓取（适用于没有 API 的平台）
# ============================================================

async def scrape_pdd_orders(username: str, password: str, save_path: str = "orders.csv"):
    """拼多多商家后台抓取订单（示例框架）"""
    async with async_playwright() as p:
        browser = await p.chromium.launch(headless=False)  # headless=True 无界面
        page = await browser.new_page()

        # 1. 登录
        await page.goto("https://mms.pinduoduo.com/login")
        await page.fill('input[placeholder="账户名"]', username)
        await page.fill('input[placeholder="密码"]', password)
        await page.click("button:has-text('登录')")
        await page.wait_for_url("**/home/**")

        # 2. 进入订单页面
        await page.goto("https://mms.pinduoduo.com/orders/list")
        await page.wait_for_selector("table")

        orders = []
        while True:
            # 3. 提取当前页订单
            rows = await page.query_selector_all("table tbody tr")
            for row in rows:
                cells = await row.query_selector_all("td")
                order = {
                    "订单号": await cells[0].inner_text(),
                    "收货人": await cells[1].inner_text(),
                    "电话": await cells[2].inner_text(),
                    "地址": await cells[3].inner_text(),
                    "商品": await cells[4].inner_text(),
                    "金额": await cells[5].inner_text(),
                    "下单时间": await cells[6].inner_text(),
                }
                orders.append(order)

            # 4. 翻页
            next_btn = await page.query_selector("li.next:not(.disabled)")
            if next_btn:
                await next_btn.click()
                await page.wait_for_timeout(2000)
            else:
                break

        await browser.close()

    df = pd.DataFrame(orders)
    df.to_csv(save_path, index=False, encoding="utf-8-sig")
    print(f"已导出 {len(orders)} 条订单到 {save_path}")
    return df


# ============================================================
# 方案二：API 对接（适用于有开放平台的平台）
# ============================================================

def fetch_taobao_orders(app_key: str, app_secret: str, session_key: str, days: int = 1):
    """淘宝开放平台 API 获取订单"""
    import hashlib
    import time as _time

    timestamp = str(int(_time.time()))
    sign_str = f"app_key{app_key}timestamp{timestamp}app_secret{app_secret}"
    sign = hashlib.md5(sign_str.encode()).hexdigest().upper()

    resp = requests.post(
        "https://eco.taobao.com/router/rest",
        data={
            "method": "taobao.trades.sold.get",
            "app_key": app_key,
            "sign": sign,
            "timestamp": timestamp,
            "session": session_key,
            "format": "json",
            "v": "2.0",
            "fields": "tid,receiver_name,receiver_mobile,receiver_address,payment,created",
        },
        timeout=30,
    )
    return resp.json()


# ============================================================
# 通用：CSV 或 API 数据统一清洗
# ============================================================

def clean_orders(df: pd.DataFrame) -> pd.DataFrame:
    """统一清洗订单数据"""
    df = df.copy()
    # 去重
    df.drop_duplicates(subset=["订单号"], inplace=True)
    # 去掉测试订单
    if "金额" in df.columns:
        df = df[df["金额"].astype(str).str.strip() != "0.00"]
    # 手机号脱敏（发给仓库用）
    if "电话" in df.columns:
        df["电话"] = df["电话"].str[:3] + "****" + df["电话"].str[-4:]
    return df


if __name__ == "__main__":
    # 示例：抓拼多多
    asyncio.run(scrape_pdd_orders("your_username", "your_password"))
