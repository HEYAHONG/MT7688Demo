.PHONY:  all

all:checkconfig
	@${MAKE} -C openwrt

.PHONY:  clean
clean:checkconfig
	@${MAKE} -C openwrt clean

.PHONY:  distclean
distclean:checkconfig
	@${MAKE} -C openwrt distclean

.PHONY:  menuconfig
menuconfig:checkconfig
	@${MAKE} -C openwrt menuconfig
	@cp -rf openwrt/.config config

.PHONY:  help
help:checkconfig
	@${MAKE} -C openwrt help

.PHONY:  checkconfig
checkconfig:checkmakefile
	@[ -f openwrt/.config ] || exit
	@[ -n "${ROOT_PATH}" ] || echo 请执行bootstrap.sh
	@[ -n "${ROOT_PATH}" ] || exit


.PHONY:  checkmakefile
checkmakefile:
	@[ -f openwrt/Makefile ] || echo 请执行bootstrap.sh
