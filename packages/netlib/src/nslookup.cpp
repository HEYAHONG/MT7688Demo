#include "netlib.h"
#include <arpa/inet.h>
#include <netdb.h>
#include "string.h"
/** \brief 通过主机名获取网络地址（第一个有效地址）
 *
 * \param isipv6 bool 是否为IPV6
 * \param host const char* 主机名
 * \return netlib_addr_t* 网络地址,失败返回NULL
 *
 */
static thread_local char nslookup_buff[64] = {0};
netlib_addr_t *netlib_nslookup(bool isipv6, const char *host)
{
    if (host == NULL)
    {
        return NULL;
    }

    struct addrinfo hint = {0};
    if (isipv6)
    {
        hint.ai_family = AF_INET6;
    }
    else
    {
        hint.ai_family = AF_INET;
    }
    hint.ai_socktype = SOCK_STREAM;

    struct addrinfo *result = NULL;
    if (0 == getaddrinfo(host, NULL, &hint, &result))
    {
        struct addrinfo *cur = result;
        if (cur)
        {
            if (cur->ai_addrlen > sizeof(nslookup_buff))
            {
                NETLIB_LOG("NSLOOKUP:internal error");
            }
            //只返回第一个结果
            memcpy(nslookup_buff, cur->ai_addr, cur->ai_addrlen);
            freeaddrinfo(result);
            return (netlib_addr_t *)&nslookup_buff;
        }
        else
        {
            return NULL;
        }

    }
    else
    {
        return NULL;
    }
}
