include $(TOPDIR)/rules.mk

PKG_NAME:=appnetlib
PKG_RELEASE:=1.0.0-$(shell git rev-parse --short HEAD)
PKG_MAINTAINER:=HEYAHONG <2229388563@qq.com>

#直接使用本地源代码
USE_SOURCE_DIR:=$(shell pwd)/src

CMAKE_BINARY_SUBDIR:=build
PKG_INSTALL:=1

CMAKE_OPTIONS:= -DOPENWRT=ON  -DPACKAGE_VERSION=${PKG_RELEASE}

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk


define Package/appnetlib
  SECTION:= Lib
  CATEGORY:= APP
  DEPENDS:=  +libstdcpp +libc +librt +libpthread
  TITLE:= netlib
endef

define Package/appnetlib/install
	$(INSTALL_DIR) $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/* $(1)/usr/lib
	$(CP) ./files/* $(1)/
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/* $(1)/usr/include
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/* $(1)/usr/lib
	$(INSTALL_DIR) $(1)/usr/lib/cmake
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/cmake/* $(1)/usr/lib/cmake
endef


$(eval $(call BuildPackage,appnetlib))
