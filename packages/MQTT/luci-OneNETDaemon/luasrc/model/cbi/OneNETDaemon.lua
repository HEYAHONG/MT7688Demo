
local fs  = require "nixio.fs"
local sys = require "luci.sys"
local uci = require "luci.model.uci".cursor()
local datatypes = require("luci.cbi.datatypes")
local _ = luci.i18n.translate

local m,s,o

m = Map("OneNETDaemon", _("OneNET(MQTT)"),_([[]]))

--远程管理MQTT设置
s=m:section(NamedSection, "MQTT", "default", _("MQTT设置"),_("MQTT服务器参数设置"))
--s.addremove=true

local function validate_address(self, value)
     local host, port = unpack(luci.util.split(value, ":"))
     if (datatypes.host(host)) then
         if port and #port then
             if not datatypes.port(port) then
                 return nil, _("请在:后输入端口号")
             end
         end
         return value
     end
     return nil, _("请输入域名或IP地址")
end
 
addr = s:option(Value, "Address", _("地址"),_("远程MQTT Broker地址 address[:port]"))
addr.datatype = "string"
addr.validate = validate_address

o = s:option(Value, "Keepalive", _("保活时间"),_("保活时间主要影响设备的离线消息"))
o.datatype = "uinteger"



o=s:option(ListValue, "SSL", _("启用SSL/TLS"),_("是否启用SSL/TLS"))
o:value("0",_("否"))
o:value("1",_("是"))
o.default="0"

o = s:option(FileUpload, "CAFile", _("CA 文件路径"),_("CA文件路径有效才能启用SSL/TLS"))
o.initial_directory = "/etc/OneNETDaemon/"


o=s:option(ListValue, "CheckServerName", _("检查服务名称"),_("检查服务名称是否与服务器证书记录的值一致,测试证书不能启用此选项"))
o:value("0",_("否"))
o:value("1",_("是"))

s=m:section(NamedSection, "OneNET", "default", _("OneNET设置"),_("OneNET参数设置"))

o = s:option(Value, "DeviceName", _("设备名称"),_("设备名称，若留空则表示使用MAC地址"))
o.datatype = "string"

local function validate_string(self, value)
	if value and #value > 0  then
		return value
	end
	return nil,_("字符串不可为空!")
end
o = s:option(Value, "ProductID", _("产品ID"),_("产品ID,不可留空"))
o.datatype = "string"
o.validate = validate_string

o = s:option(Value, "AccessKey", _("访问密钥"),_("访问密钥，可使用产品Key也可使用设备Key,不可留空"))
o.datatype = "string"
o.validate = validate_string





return m
