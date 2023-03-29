include $(TOPDIR)/rules.mk

PKG_NAME:=FirstBootInit
PKG_RELEASE:=1.0.0-$(shell git rev-parse --short HEAD)
PKG_MAINTAINER:=HEYAHONG <2229388563@qq.com>


include $(INCLUDE_DIR)/package.mk


define Package/FirstBootInit
  SECTION:= FirstBootInit
  CATEGORY:= Base system
  TITLE:= Execute script when firstboot
endef

define Build/Prepare
        mkdir -p $(PKG_BUILD_DIR)
endef

define Build/Compile/Default

endef
Build/Compile = $(Build/Compile/Default)


define Package/FirstBootInit/install
	$(CP) ./files/* $(1)/
endef

$(eval $(call BuildPackage,FirstBootInit))
