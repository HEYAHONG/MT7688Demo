
local fs  = require "nixio.fs"
local sys = require "luci.sys"
local uci = require "luci.model.uci".cursor()
local datatypes = require("luci.cbi.datatypes")
local _ = luci.i18n.translate

local m,s,o

m = Map("MQTTDaemon", _("MQTT客户端配置页面"),_([[]]))

--远程管理MQTT设置
s=m:section(NamedSection, "MQTTDaemon", "default", _("MQTT Broker地址设置"),_("远程管理MQTT Broker服务器地址"))
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

s:option(Value, "ClientId", _("客户端ID"),_("若为空则使用序列号"))

s:option(Value, "UserName", _("用户名"),_("若为空则使用序列号"))

o = s:option(Value, "PassWord", _("密码"),_("若为空则使用序列号"))
o.password = true


o=s:option(ListValue, "SSL", _("启用SSL/TLS"),_("是否启用SSL/TLS"))
o:value("0",_("否"))
o:value("1",_("是"))
o.default="0"

o = s:option(FileUpload, "CAFile", _("CA 文件路径"),_("CA文件路径与CA搜索目录任一有效才能启用SSL/TLS"))
o.initial_directory = "/etc/MQTTDaemon/"


o = s:option(Value, "CAPath", _("CA 搜索目录"),_("CA文件路径与CA搜索目录任一有效才能启用SSL/TLS"))
o.datatype = "directory"
o.default="/etc/MQTTDaemon/"

o = s:option(FileUpload, "CertFile", _("证书(PEM格式)"),_("证书与证书密钥需要同时有效才能启用SSL/TLS双向认证"))
o.initial_directory = "/etc/MQTTDaemon/"


o = s:option(FileUpload, "KeyFile", _("证书密钥(PEM格式)"),_("证书与证书密钥需要同时有效才能启用SSL/TLS双向认证"))
o.initial_directory = "/etc/MQTTDaemon/"

o=s:option(ListValue, "CheckServerName", _("检查服务名称"),_("检查服务名称是否与服务器证书记录的值一致,测试证书不能启用此选项"))
o:value("0",_("否"))
o:value("1",_("是"))


return m
