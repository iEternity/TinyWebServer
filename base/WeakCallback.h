//
// Created by zhangkuo on 17-9-22.
//

#ifndef WEBSERVER_WEAKCALLBACK_H
#define WEBSERVER_WEAKCALLBACK_H

#include <memory>
#include <functional>

namespace WebServer
{

template<typename CLASS, typename... ARGS>
class WeakCallback
{
public:
    using Function = std::function<void(CLASS*, ARGS...)>;
    using Object   = std::weak_ptr<CLASS>;

    WeakCallback(const Object& object,
                 const Function & function):
            object_(object),
            function_(function)
    {
    }

    void operator() (ARGS&&... args) const
    {
        std::shared_ptr<CLASS> ptr(object_.lock());
        if(ptr)
        {
            function_(ptr.get(), std::forward<ARGS>(args)...);
        }
    }

private:
    Object object_;
    Function function_;
};

template<typename CLASS, typename... ARGS>
WeakCallback<CLASS, ARGS...> makeWeakCallback(const std::shared_ptr<CLASS>& object,
                                              void(CLASS::*function)(ARGS...))
{
    return WeakCallback<CLASS, ARGS...>(object, function);
}

template<typename CLASS, typename... ARGS>
void makeWeakCallback(const std::shared_ptr<CLASS>& object, void(CLASS::*function)(ARGS...) const)
{
    return WeakCallback<CLASS, ARGS...>(object, function);
};

}

#endif //WEBSERVER_WEAKCALLBACK_H
