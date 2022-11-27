#!/bin/bash

#检查工具是否存在,$1为待检查的工具名。
function CheckTool
{
	[  -n "$1"  ] ||
	{
		echo -e  "\033[41;37mCheckTool 参数错误!!\033[40;37m";
		return 255;
	};
	ToolPath=`which $1`;
	[ -e "$ToolPath" ] ||
	{
		 echo -e "\033[41;37m$1 不存在，请先安装此工具\033[40;37m";
		 return 255;
	};
	return 0;
}

#检查必要的工具
CheckTool git
[ $? -eq 0 ] || exit;
CheckTool install
[ $? -eq 0 ] || exit;
CheckTool patch
[ $? -eq 0 ] || exit;
CheckTool find
[ $? -eq 0 ] || exit;
CheckTool dirname
[ $? -eq 0 ] || exit;
CheckTool readlink
[ $? -eq 0 ] || exit;
CheckTool ln
[ $? -eq 0 ] || exit;
CheckTool sed
[ $? -eq 0 ] || exit;

#设置ROOT_PATH变量

self_path=""

# shellcheck disable=SC2128  # ignore array expansion warning
if [ -n "${BASH_SOURCE-}" ]
then
self_path="${BASH_SOURCE}"
elif [ -n "${ZSH_VERSION-}" ]
then
self_path="${(%):-%x}"
else
echo -e "\033[41;37m不能获取工作目录\033[40;37m";
return 1
fi

# shellcheck disable=SC2169,SC2169,SC2039  # unreachable with 'dash'
if [[ "$OSTYPE" == "darwin"* ]]; then
# convert possibly relative path to absolute
script_dir="$(realpath_int "${self_path}")"
# resolve any ../ references to make the path shorter
script_dir="$(cd "${script_dir}" || exit 1; pwd)"
else
# convert to full path and get the directory name of that
script_name="$(readlink -f "${self_path}")"
script_dir="$(dirname "${script_name}")"
fi

export ROOT_PATH="${script_dir}";


#下载缺失的源码
echo -e  "\033[44;37m下载openwrt\033[40;37m";
git submodule  update --init --recursive --force
[ $? -eq 0 ] || exit;

#清理openwrt目录
echo -e "\033[44;37m清理openwrt\033[40;37m";
cd $ROOT_PATH/openwrt
git clean -f
[ $? -eq 0 ] || exit;
git reset --hard
[ $? -eq 0 ] || exit;
cd $ROOT_PATH

#安装支持补丁
echo -e "\033[44;37m安装支持补丁\033[40;37m";
cd $ROOT_PATH/patches
for i in `find -name "*.patch"`
do
echo applying $i
patch  -p1 -d $ROOT_PATH/openwrt < $ROOT_PATH/patches/$i;
[ $? -eq 0 ] || exit;
done
cd $ROOT_PATH

#复制文件
echo -e "\033[44;37m复制文件\033[40;37m";
#复制config文件
cp $ROOT_PATH/config $ROOT_PATH/openwrt/.config
[ $? -eq 0 ] || exit;
#复制key-build
cp $ROOT_PATH/key-build $ROOT_PATH/openwrt/key-build
[ $? -eq 0 ] || exit;
cp $ROOT_PATH/key-build.pub $ROOT_PATH/openwrt/key-build.pub
[ $? -eq 0 ] || exit;
cd $ROOT_PATH

#安装package
echo -e "\033[44;37m安装packages\033[40;37m";
mkdir -p $ROOT_PATH/openwrt/package/feeds/main
for i in `find $ROOT_PATH/packages -name "Makefile" `
do
	PACKAGE_DIR=`dirname $i`;
	if [ -d $PACKAGE_DIR ]
	then
		echo "install $PACKAGE_DIR"
		ln -sf $PACKAGE_DIR $ROOT_PATH/openwrt/package/feeds/main/
		[ $? -eq 0 ] || exit;
	fi
done
cd $ROOT_PATH

#调整版本信息
echo -e "\033[44;37m调整版本信息\033[40;37m";
Version=`git show   --pretty=tformat:%as | sed -n 1p`
GitVersion=`git rev-parse --short HEAD`
echo "Version=$Version,GitVerison=$GitVersion"
sed -i "s/CONFIG_VERSION_NUMBER=.*/CONFIG_VERSION_NUMBER=\"$Version\"/g" $ROOT_PATH/openwrt/.config
sed -i "s/CONFIG_VERSION_CODE=.*/CONFIG_VERSION_CODE=\"$GitVersion\"/g" $ROOT_PATH/openwrt/.config

#更新并安装openwrt feed
echo -e "\033[44;37m安装openwrt feed\033[40;37m";
cd $ROOT_PATH/openwrt
./scripts/feeds update -a
[ $? -eq 0 ] || exit;
./scripts/feeds install -a
[ $? -eq 0 ] || exit;

#软件包黑名单
for i in `cat $ROOT_PATH/package.blacklist`
do
	./scripts/feeds uninstall $i
done
cd $ROOT_PATH

echo -e "\033[44;37m准备工作已完成,请使用编译(make=编译;make menuconfig=配置)\033[40;37m";
exec ${SHELL}

