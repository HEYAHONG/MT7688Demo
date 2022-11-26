
include $(TOPDIR)/rules.mk

PKG_NAME:=wqy-zenhei
PKG_RELEASE:=1.0.0

PKG_SOURCE:=$(PKG_NAME)-0.9.45.tar.gz
PKG_SOURCE_URL:=@SF/wqy
PKG_HASH:=e4b7e306475bf9427d1757578f0e4528930c84c44eaa3f167d4c42f110ee75d6
PKG_LICENSE_FILES:=COPYING
PKG_MAINTAINER:=HEYAHONG <2229388563@qq.com>


include $(INCLUDE_DIR)/package.mk


define Package/wqy-zenhei
  TITLE:=wqy-zenhei-font-ttc
  SECTION:=fonts
  CATEGORY:=Fonts
endef

define Build/Compile
		true
endef

define Build/Configure
		true
endef

define Package/wqy-zenhei/install
		$(INSTALL_DIR) $(1)/usr/share/fonts/ttf-wqy-zenhei
		$(CP)  $(PKG_BUILD_DIR)/*.ttc   $(1)/usr/share/fonts/ttf-wqy-zenhei/
endef


$(eval $(call BuildPackage,wqy-zenhei))
