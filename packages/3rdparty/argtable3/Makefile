
include $(TOPDIR)/rules.mk

PKG_NAME:=argtable3
PKG_RELEASE:=1

PKG_SOURCE_URL:= https://github.com/argtable/argtable3.git
PKG_SOURCE_PROTO:=git
PKG_SOURCE_DATE:=2022-05-23
PKG_SOURCE_VERSION:=6f0e40bc44c99af353ced367c6fafca8705f5fca
PKG_MIRROR_HASH:=e6d784cffa3521233fb452b32ba6ecfa0721c44642a6df568ef4907ee294a1cb
PKG_LICENSE_FILES:=LICENSE

PKG_MAINTAINER:=HEYAHONG <2229388563@qq.com>

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk


define Package/argtable3
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=A single-file, ANSI C, command-line parsing library that parses GNU-style command-line options.
endef

TARGET_CFLAGS += -I$(STAGING_DIR)/usr/include
TARGET_LDFLAGS += -L$(STAGING_DIR)/usr/lib

CMAKE_OPTIONS += -DBUILD_SHARED_LIBS=ON -DARGTABLE3_ENABLE_TESTS=OFF -DARGTABLE3_ENABLE_EXAMPLES=OFF

define Package/argtable3/install
	$(INSTALL_DIR) $(1)/lib
	$(CP) $(PKG_BUILD_DIR)/src/libargtable3.so* $(1)/lib/
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/src/argtable3.h $(1)/usr/include
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/src/libargtable3.so* $(1)/usr/lib
	$(INSTALL_DIR) $(1)/usr/lib/cmake
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/cmake/* $(1)/usr/lib/cmake
endef

$(eval $(call BuildPackage,argtable3))
