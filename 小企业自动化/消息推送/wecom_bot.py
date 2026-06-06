"""
企业微信机器人推送
开通方式：企业微信管理后台 → 应用管理 → 群机器人 → 获取 Webhook URL
"""

import requests
import json
from pathlib import Path
from typing import Optional


class WeComBot:
    def __init__(self, webhook_url: str):
        self.url = webhook_url

    def send_text(self, content: str, mentioned_list: Optional[list] = None):
        """发送文本消息，可 @指定人"""
        data = {
            "msgtype": "text",
            "text": {
                "content": content,
                "mentioned_list": mentioned_list or [],
            },
        }
        return requests.post(self.url, json=data, timeout=10).json()

    def send_markdown(self, content: str):
        """发送 Markdown 格式消息"""
        data = {
            "msgtype": "markdown",
            "markdown": {"content": content},
        }
        return requests.post(self.url, json=data, timeout=10).json()

    def send_image(self, image_path: str):
        """发送图片（需先上传得到 base64）"""
        import base64
        with open(image_path, "rb") as f:
            b64 = base64.b64encode(f.read()).decode()

        # 先计算 md5
        import hashlib
        md5 = hashlib.md5(open(image_path, "rb").read()).hexdigest()
        data = {
            "msgtype": "image",
            "image": {"base64": b64, "md5": md5},
        }
        return requests.post(self.url, json=data, timeout=10).json()

    def send_file(self, file_path: str):
        """发送文件"""
        # 企业微信文件发送需要通过 upload 接口获取 media_id
        upload_url = self.url.replace("send?key=", "upload_media?key=", 1) \
            if "send?key=" in self.url else self.url
        with open(file_path, "rb") as f:
            resp = requests.post(upload_url, files={"media": (Path(file_path).name, f)})
        media_id = resp.json().get("media_id")
        if media_id:
            data = {
                "msgtype": "file",
                "file": {"media_id": media_id},
            }
            return requests.post(self.url, json=data, timeout=10).json()
        return resp.json()

    def send_news(self, title: str, description: str, url: str, picurl: str = ""):
        """发送图文消息"""
        data = {
            "msgtype": "news",
            "news": {
                "articles": [{
                    "title": title,
                    "description": description,
                    "url": url,
                    "picurl": picurl,
                }]
            },
        }
        return requests.post(self.url, json=data, timeout=10).json()


# ============================================================
# 开箱即用的业务通知模版
# ============================================================

def notify_new_order(bot: WeComBot, order: dict):
    """新订单通知"""
    msg = (
        f"🛒 **新订单提醒**\n"
        f"订单号：{order['订单号']}\n"
        f"商品：{order.get('商品', '')}\n"
        f"金额：¥{order.get('金额', '')}\n"
        f"收货人：{order.get('收货人', '')}\n"
        f"时间：{order.get('下单时间', '')}"
    )
    return bot.send_markdown(msg)


def notify_daily_summary(bot: WeComBot, stats: dict):
    """每日汇总通知"""
    msg = (
        f"📊 **{stats['日期']} 运营日报**\n"
        f"订单数：<font color='info'>{stats['订单数']} 单</font>\n"
        f"销售额：<font color='warning'>¥{stats['销售额']:,.2f}</font>\n"
        f"发货数：{stats['发货数']} 单\n"
        f"退款：{stats.get('退款', 0)} 单"
    )
    return bot.send_markdown(msg)


def notify_alert(bot: WeComBot, title: str, detail: str):
    """异常告警"""
    msg = (
        f"🚨 <font color='warning'>**{title}**</font>\n"
        f"{detail}\n"
        f"时间：{__import__('datetime').datetime.now().strftime('%H:%M:%S')}"
    )
    return bot.send_markdown(msg)


if __name__ == "__main__":
    WEBHOOK = "https://qyapi.weixin.qq.com/cgi-bin/webhook/send?key=你的KEY"
    bot = WeComBot(WEBHOOK)
    bot.send_text("测试消息：自动化系统已启动 ✓")
