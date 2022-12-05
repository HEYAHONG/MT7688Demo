#include "args.h"
#include "log.h"
#include "argtable3.h"
#include "string.h"
#include <map>
#include <string>
#include <fstream>
static std::map<std::string, std::string> args;

/*
检查字符串,去除开头与结尾的空格 " '
*/
static std::string string_check(std::string value)
{

    while (value.length() > 0 && (value.c_str()[0] == ' ' || value.c_str()[0] == '\"' || value.c_str()[0] == '\''))
    {
        value = value.substr(1);
    }

    while (value.length() > 0 && (value.c_str()[value.length() - 1] == ' ' || value.c_str()[value.length() - 1] == '\"' || value.c_str()[value.length() - 1] == '\''))
    {
        value = value.substr(0, value.length() - 1);
    }

    return value;
}

/*
将含有等号的字符串分为两个字符串
*/
static std::pair<std::string, std::string> string_split(std::string value)
{
    std::pair<std::string, std::string> ret;
    if (!value.empty())
    {
        if (value.find("=") != std::string::npos)
        {
            ret.first = value.substr(0, value.find("="));
            ret.second = value.substr(value.find("=") + 1);
        }
        else
        {
            ret.first = value;
        }
    }

    ret.first = string_check(ret.first);
    ret.second = string_check(ret.second);

    return ret;
}

void args_parse(int argc, char *argv[])
{

    /*
    参数信息
    */
    struct arg_file *configfile = arg_filen("f", "file", "config.conf", 0, 1, "配置文件路径(类ini格式，忽略节)。");
    struct arg_str *settings = arg_strn("s", "setting", "Key=Value", 0, 256, "可覆盖配置文件中的配置,可多次输入。");
    struct arg_lit *help = arg_lit0("h", "help", "打印帮助");
    void *argtable[] =
    {
        configfile,
        settings,
        help,
        arg_end(100),
    };

    arg_parse(argc, argv, argtable);

    if (configfile->count > 0)
    {
        //配置文件
        std::string filepath = configfile->filename[0];
        if (!filepath.empty())
        {
            std::fstream file;
            file.open(filepath, std::ios::in);
            if (file.is_open())
            {
                while (!file.eof())
                {
                    std::string data;
                    {
                        char buff[4096] = {0};
                        file.getline(buff, sizeof(buff) - 1);
                        if (strlen(buff) != 0)
                        {
                            for (size_t i = 0; i < sizeof(buff); i++)
                            {
                                if (buff[i] != ' ' || buff[i] == '\0')
                                {
                                    data = std::string(&buff[i]);
                                    break;
                                }
                            }

                            if (data.empty())
                            {
                                continue;
                            }

                        }
                    }

                    if (data.length() > 0 && data.c_str()[0] == ';')
                    {
                        //跳过注释
                        continue;
                    }

                    if (data.length() > 0 && data.c_str()[0] == '#')
                    {
                        //跳过注释
                        continue;
                    }

                    auto s_pair = string_split(data);
                    if (!s_pair.first.empty())
                        args[s_pair.first] = s_pair.second;
                }

                file.close();
            }
        }
    }


    if (settings->count > 0)
    {
        for (size_t i = 0; i < settings->count; i++)
        {
            auto s_pair = string_split(settings->sval[i]);
            if (!s_pair.first.empty())
                args[s_pair.first] = s_pair.second;
        }
    }
    if (help->count > 0)
    {
        arg_print_glossary_gnu(stdout, argtable);
        exit(0);//退出
    }



    /*
    释放参数信息
    */
    arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
}

void args_print()
{
    for (auto arg : args)
    {
        LOGINFO("ARG -> %s=%s ", arg.first.c_str(), arg.second.c_str());
    }
}

size_t args_size()
{
    return args.size();
}

static thread_local std::string key;
const char *args_getkey(size_t index)
{
    if (index >= args_size())
    {
        return NULL;
    }

    auto it = args.begin();

    std::advance(it, index);

    if (it != args.end())
    {
        key = it->first;
        return key.c_str();
    }

    return NULL;
}

static thread_local std::string value;
const char *args_get(const char *key)
{
    if (key == NULL || strlen(key) == 0)
    {
        return NULL;
    }
    if (args.find(key) != args.end())
    {
        value = args[key];
        return value.c_str();
    }

    return NULL;
}
