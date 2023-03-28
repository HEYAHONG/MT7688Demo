
include $(TOPDIR)/rules.mk

PKG_NAME:=libserialport
PKG_RELEASE:=1

PKG_SOURCE_URL:= https://github.com/sigrokproject/libserialport.git
PKG_SOURCE_PROTO:=git
PKG_SOURCE_DATE:=2023-03-28
PKG_SOURCE_VERSION:=6f9b03e597ea7200eb616a4e410add3dd1690cb1
PKG_MIRROR_HASH:=bc7e1e6eee33955d7b1550fd060e463fa5fa243ea8318983c739d60123ba0ef4
PKG_LICENSE_FILES:=COPYING

PKG_MAINTAINER:=HEYAHONG <2229388563@qq.com>
PKG_FIXUP:=libtool autoreconf 
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/autotools.mk

define Package/libserialport
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=libserialport: cross-platform library for accessing serial ports
endef

TARGET_CFLAGS += -I$(STAGING_DIR)/usr/include
TARGET_LDFLAGS += -L$(STAGING_DIR)/usr/lib


define Package/libserialport/install
	$(INSTALL_DIR) $(1)/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/*.so* $(1)/lib/
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/*.h $(1)/usr/include
	$(INSTALL_DIR) $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/*.so* $(1)/usr/lib
	$(INSTALL_DIR) $(1)/usr/lib/pkgconfig
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/*.pc $(1)/usr/lib/pkgconfig
endef

$(eval $(call BuildPackage,libserialport))
