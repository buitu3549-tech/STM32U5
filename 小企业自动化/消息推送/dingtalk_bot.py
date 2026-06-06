"""
钉钉机器人推送
开通方式：钉钉群 → 群设置 → 智能群助手 → 添加机器人 → Webhook
支持加签安全模式（推荐）
"""

import requests
import time
import hmac
import hashlib
import base64
import urllib.parse
from typing import Optional


class DingTalkBot:
    def __init__(self, webhook_url: str, secret: str = ""):
        self.url = webhook_url
        self.secret = secret

    def _sign(self):
        """生成加签参数（安全模式下需要）"""
        if not self.secret:
            return ""
        timestamp = str(round(time.time() * 1000))
        sign_str = f"{timestamp}\n{self.secret}"
        hmac_code = hmac.new(
            self.secret.encode("utf-8"),
            sign_str.encode("utf-8"),
            digestmod=hashlib.sha256
        ).digest()
        sign = urllib.parse.quote_plus(base64.b64encode(hmac_code))
        return f"&timestamp={timestamp}&sign={sign}"

    def _post(self, data: dict):
        url = self.url + self._sign()
        return requests.post(url, json=data, timeout=10).json()

    def send_text(self, content: str, at_mobiles: Optional[list] = None, at_all: bool = False):
        """发送文本消息"""
        data = {
            "msgtype": "text",
            "text": {"content": content},
            "at": {
                "atMobiles": at_mobiles or [],
                "isAtAll": at_all,
            },
        }
        return self._post(data)

    def send_markdown(self, title: str, text: str):
        """发送 Markdown 消息"""
        data = {
            "msgtype": "markdown",
            "markdown": {"title": title, "text": text},
        }
        return self._post(data)

    def send_link(self, title: str, text: str, message_url: str, pic_url: str = ""):
        """发送链接卡片"""
        data = {
            "msgtype": "link",
            "link": {
                "title": title,
                "text": text,
                "messageUrl": message_url,
                "picUrl": pic_url,
            },
        }
        return self._post(data)

    def send_feed_card(self, links: list):
        """发送多链接卡片（最多 5 个）"""
        data = {
            "msgtype": "feedCard",
            "feedCard": {"links": links},
        }
        return self._post(data)


# ============================================================
# 业务通知模板
# ============================================================

def dingtalk_notify_new_order(bot: DingTalkBot, order: dict):
    """钉钉新订单通知"""
    title = f"新订单 #{order['订单号']}"
    text = (
        f"### 🛒 新订单提醒\n\n"
        f"**订单号**：{order['订单号']}  \n"
        f"**商品**：{order.get('商品', '')}  \n"
        f"**金额**：¥{order.get('金额', '')}  \n"
        f"**收货人**：{order.get('收货人', '')}  \n"
        f"**下单时间**：{order.get('下单时间', '')}  \n"
    )
    return bot.send_markdown(title, text)


def dingtalk_notify_daily(bot: DingTalkBot, stats: dict):
    """钉钉每日经营日报"""
    title = f"📊 {stats['日期']} 日报"
    text = (
        f"### 📊 {stats['日期']} 经营日报\n\n"
        f"---\n\n"
        f"- 订单数：**{stats['订单数']}** 单\n"
        f"- 销售额：**¥{stats['销售额']:,.2f}**\n"
        f"- 已发货：**{stats['发货数']}** 单\n"
        f"- 退款：**{stats.get('退款', 0)}** 单\n"
        f"- 客单价：**¥{stats.get('客单价', 0):,.2f}**\n"
    )
    return bot.send_markdown(title, text)


if __name__ == "__main__":
    WEBHOOK = "https://oapi.dingtalk.com/robot/send?access_token=你的TOKEN"
    SECRET = "你的加签密钥"  # 为空则不启用加签
    bot = DingTalkBot(WEBHOOK, SECRET)
    bot.send_text("测试：自动化系统已启动 ✓")
