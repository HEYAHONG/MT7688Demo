include $(TOPDIR)/rules.mk

PKG_NAME:=cli
PKG_RELEASE:=1.0.0-$(shell git rev-parse --short HEAD)
PKG_MAINTAINER:=HEYAHONG <2229388563@qq.com>

#直接使用本地源代码
USE_SOURCE_DIR:=$(shell pwd)/src

CMAKE_BINARY_SUBDIR:=build
PKG_INSTALL:=1

CMAKE_OPTIONS:= -DOPENWRT=ON  -DPACKAGE_VERSION=${PKG_RELEASE}

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk


define Package/cli
  SECTION:= CLI
  CATEGORY:= APP
  DEPENDS:=  +libstdcpp +libc +librt +libpthread +appnetlib
  TITLE:= cli for app
endef

define Package/cli/install
	$(INSTALL_DIR) $(1)/usr/bin/
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/* $(1)/usr/bin
	$(CP) ./files/* $(1)/
endef

$(eval $(call BuildPackage,cli))
