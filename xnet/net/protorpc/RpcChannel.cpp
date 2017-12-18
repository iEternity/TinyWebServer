//
// Created by zhangkuo on 17-12-9.
//
#include <xnet/net/protorpc/RpcChannel.h>
#include <xnet/base/Logging.h>
#include <xnet/net/protorpc/rpc.pb.h>

using namespace std;
using namespace std::placeholders;
using namespace xnet;

RpcChannel::RpcChannel():
    codec_(std::bind(&RpcChannel::onRpcMessage, this, _1, _2, _3)),
    services_(nullptr)
{
    LOG_INFO << "RpcChannel::ctor - " << this;
}

RpcChannel::RpcChannel(const TcpConnectionPtr& conn):
    codec_(std::bind(&RpcChannel::onRpcMessage, this, _1, _2, _3)),
    conn_(conn),
    services_(nullptr)
{
    LOG_INFO << "RpcChannel::ctor - " << this;
}

RpcChannel::~RpcChannel()
{
    LOG_INFO << "RpcChannel::dtor - " << this;
    for(auto& it : outstandings_)
    {
        OutstandingCall out = it.second;
        delete out.response;
        delete out.done;
    }
}

void RpcChannel::CallMethod(const ::google::protobuf::MethodDescriptor *method,
                            ::google::protobuf::RpcController *controller,
                            const ::google::protobuf::Message *request,
                            ::google::protobuf::Message *response,
                            ::google::protobuf::Closure *done)
{
    RpcMessage message;
    message.set_type(REQUEST);
    int64_t id = ++id_;
    message.set_id(id);
    message.set_service(method->service()->full_name());
    message.set_method(method->name());
    message.set_request(request->SerializeAsString());

    OutstandingCall out = { response, done};
    {
        std::lock_guard<mutex> lock(mutex_);
        outstandings_[id] = out;
    }
    codec_.send(conn_, message);
}

void RpcChannel::onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp receiveTime)
{
    codec_.onMessage(conn, buf, receiveTime);
}

void RpcChannel::onRpcMessage(const TcpConnectionPtr& conn, const RpcMessagePtr& messagePtr, Timestamp receiveTime)
{
    assert(conn == conn_);
    RpcMessage& message = *messagePtr;
    if(message.type() == RESPONSE)
    {
        handleResponse(message);
    }
    else if(message.type() == REQUEST)
    {
        handleRequest(message);

    }
    else if(message.type() == ERROR)
    {
        LOG_ERROR << "onRpcMessage type error";
    }
}

void RpcChannel::handleResponse(const RpcMessage& message)
{
    int64_t id = message.id();
    OutstandingCall out = { nullptr, nullptr };

    {
        lock_guard<mutex> lock(mutex_);
        auto it = outstandings_.find(id);
        if(it != outstandings_.end())
        {
            out = it->second;
            outstandings_.erase(it);
        }
    }

    if(out.response)
    {
        unique_ptr<google::protobuf::Message> d(out.response);
        if(message.has_response())
        {
            out.response->ParseFromString(message.response());
        }
        if(out.done)
        {
            out.done->Run();
        }
    }
}

void RpcChannel::handleRequest(const RpcMessage& message)
{
    ErrorCode error = WRONG_PROTO;
    if(services_)
    {
        const auto it = services_->find(message.service());
        if(it != services_->end())
        {
            google::protobuf::Service* service = it->second;
            const auto desc = service->GetDescriptor();
            const auto method = desc->FindMethodByName(message.method());
            if(method)
            {
                unique_ptr<google::protobuf::Message> request(service->GetRequestPrototype(method).New());
                if(request->ParseFromString(message.request()))
                {
                    auto response = service->GetResponsePrototype(method).New();
                    int64_t id = message.id();
                    service->CallMethod(method, nullptr, request.get(), response,
                                        google::protobuf::NewCallback(this, &RpcChannel::doneCallback, response, id));
                    error = NO_ERROR;
                }
                else
                {
                    error = INVALID_REQUEST;
                }
            }
            else
            {
                error = NO_METHOD;
            }
        }
        else
        {
            error = NO_SERVICE;
        }
    }
    else
    {
        error = NO_SERVICE;
    }

    if(error == NO_ERROR)
    {
        RpcMessage response;
        response.set_type(RESPONSE);
        response.set_id(message.id());
        response.set_error(error);
        codec_.send(conn_, response);
    }
}

void RpcChannel::doneCallback(::google::protobuf::Message* response, int64_t id)
{
    unique_ptr<google::protobuf::Message> d(response);
    RpcMessage message;
    message.set_type(RESPONSE);
    message.set_id(id);
    message.set_response(response->SerializeAsString());
    codec_.send(conn_, message);
}