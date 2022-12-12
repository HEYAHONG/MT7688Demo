#!/bin/bash

#选择qemu user
export QEMU_USER=qemu-mipsel

#设定CPU类型
export QEMU_CPU=24KEc

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
CheckTool ${QEMU_USER}
[ $? -eq 0 ] || exit;


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
ROOT_PATH="${script_dir}";

#设定库前缀（目标root目录）
export QEMU_LD_PREFIX=${ROOT_PATH}/openwrt/staging_dir/target-mipsel_24kc_musl/root-ramips

#设定使用的环境变量
export QEMU_SET_ENV=LD_LIBRARY_PATH=/lib:/usr/lib:/usr/local/lib:. 

#设定不使用的环境变量
export QEMU_UNSET_ENV=LD_PRELOAD,LD_DEBUG,QEMU_STACK_SIZE

#执行qemu
exec ${QEMU_USER}  $*
