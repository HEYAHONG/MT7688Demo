include $(TOPDIR)/rules.mk

LUCI_TITLE:= MQTTDaemon  LuCI interface
LUCI_DEPENDS:=+luci-compat +luci-mod-admin-full MQTTDaemon
LUCI_DESCRIPTION:= LuCI interface for MQTTDaemon

PKG_MAINTAINER:= 2229388563@qq.com
PKG_VERSION:=1.0.0-$(shell git rev-parse --short HEAD)
PKG_LICENSE:= MIT

include $(TOPDIR)/feeds/luci/luci.mk

# call BuildPackage - OpenWrt buildroot signature
