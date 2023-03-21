include $(TOPDIR)/rules.mk

PKG_NAME:=OneNETDaemon
PKG_RELEASE:=1.0.0-$(shell git rev-parse --short HEAD)
PKG_MAINTAINER:=HEYAHONG <2229388563@qq.com>

#直接使用本地源代码
USE_SOURCE_DIR:=$(shell pwd)/src

CMAKE_BINARY_SUBDIR:=build
PKG_INSTALL:=1

CMAKE_OPTIONS:= -DOPENWRT=ON  -DPACKAGE_VERSION=${PKG_RELEASE}

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk


define Package/OneNETDaemon
  SECTION:= OneNETDaemon
  CATEGORY:= APP
  DEPENDS:=  +libstdcpp +libc +librt +libpthread +appnetlib  +argtable3 +libmosquitto-ssl +libmbedtls +jsoncpp
  TITLE:= OneNETDaemon (MQTT)
endef

define Package/OneNETDaemon/install
	$(INSTALL_DIR) $(1)/usr/bin/
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/* $(1)/usr/bin
	$(CP) ./files/* $(1)/
endef

$(eval $(call BuildPackage,OneNETDaemon))
