//
// Created by zheng on 2020/4/8.
//
#include <unistd.h>
#include <netdb.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cassert>

#include "SocketClient.h"

#define SA struct sockaddr

SocketClient::SocketClient(char *addr, int port)
{
    int ret;
    strncpy(this->addr, addr, 99);
    this->port=port;
    msg_size = 0;
    ret = this->connect_to_server();
}

int SocketClient::connect_to_server()
{
    struct sockaddr_in servaddr;

    // socket create and varification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        return -1;
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(addr);
    servaddr.sin_port = htons(port);

    // connect_to_server the client socket to server socket
    if (::connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) != 0) {
        printf("connection with the server failed...\n");
        return -1;
    }
    else
        printf("connected to the server..\n");
    return 0;
}

int SocketClient::send()
{
    assert(msg_size>0);
    ssize_t ret = write(sockfd, buf, msg_size);
    if (ret<msg_size)
    {
        printf("write %d ret %ld\n", msg_size, ret);
    }
//    bzero(buf, sizeof(buf));
    msg_size = 0;
    return 0;
}

int
SocketClient::serialize(int rank, unsigned long begin, unsigned long end, const Graph &graph_calc,
                        const Graph &graph_comm, const Graph &graph_io)
{
    Header header = {};
    auto sGraphKey = reinterpret_cast<SGraphKey*>(buf + sizeof(Header));
    header.rank = rank, header.begin = begin, header.end = end;
    int cnt_key=0, cnt_value=0;
    for (const auto &kv:graph_calc)
    {
        // buffer size limit
        if (sizeof(Header) + sizeof(SGraphKey) * (cnt_key + 1) +
            sizeof(SGraphValue) * (cnt_value + kv.second.size()) >= MAXBUFSIZ)
        {
//            fprintf(stderr, "Send buf truncated. c_k=%d c_v=%d\n", cnt_key,
//                    cnt_value);
            break;
        }
        sGraphKey[cnt_key].first = kv.first.first;
        sGraphKey[cnt_key].second = kv.first.second;
        sGraphKey[cnt_key].num=kv.second.size();
        cnt_key++;
        cnt_value+=kv.second.size();
    }
    int i_key=0;
    auto *sDataType = reinterpret_cast<SGraphValue *>(buf + sizeof(Header) +
                                                         sizeof(SGraphKey) *
                                                         cnt_key);
    header.n_keys = cnt_key;
    this->msg_size = header.msg_size =
            sizeof(Header) + sizeof(SGraphKey) * cnt_key +
            sizeof(SGraphValue) * cnt_value;

    memcpy(buf, &header, sizeof(Header));
    for (const auto &kv:graph_calc)
    {
        if (i_key >= cnt_key)
        {
            // fprintf(stderr, "Warning: exceed buf %d/%d\n", i_key, cnt_key);
            break;
        }
        memcpy(sDataType, kv.second.data(),
               sizeof(DataType) * kv.second.size());
        sDataType += kv.second.size();
        i_key++;
    }
//    fprintf(stderr, "Msgsize=%d Header rk=%d nkeys=%d nval=%d\n",
//            this->msg_size, header.rank, header.n_keys, cnt_value);
    return 0;
}

//void SGraphValue::load_from_DataType(const DataType &dataType)
//{
//    this->timestamp=dataType.timestamp;
//    this->elapsed=dataType.elapsed;
//}
