#pragma once
#include <map>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <xnet/base/Types.h>
#include <xnet/base/Timestamp.h>

namespace xnet
{

class HttpRequest
{
public:
    enum class Method
    {
        kInvalid, kGet, kPost, kPut, kDelete, kHead
    };

    enum class Version
    {
        kUnknown, kHttp10, kHttp11
    };
public:
    HttpRequest()
        : method_(Method::kInvalid),
          version_(Version::kUnknown)
    {
    }

    void    setVersion(Version v) { version_ = v; }
    Version getVersion() const { return version_; }

    bool setMethod(const string& s)
    {
        std::map<string, Method> m =
		{
			{ "GET",	Method::kGet },
			{ "POST",	Method::kPost },
			{ "PUT",	Method::kPut },
			{ "DELETE", Method::kDelete },
			{ "HEAD",	Method::kHead }
		};
		if (m.find(s) != m.end())
		{
			method_ = m[s];
		}
		else
		{
			method_ = Method::kInvalid;
		}

		return method_ != Method::kInvalid;
    }
    Method getMethod() const { return method_; }
    string methodString()
    {
        std::map<Method, string> m =
        {
            {Method::kGet,      "GET"},
            {Method::kPost,     "POST"},
            {Method::kPut,      "PUT"},
            {Method::kDelete,   "DELETE"},
            {Method::kHead,     "HEAD"},
            {Method::kInvalid,  "UNKNOWN"}
        };

        return m[method_];
    }

    void setQuery(const string& s) 
    { 
        query_ = s; 
    }
    void setQuery(string&& s)
    {
        query_ = std::move(s);
    }
    string getQuery() const { return query_; }

    void setPath(const string& s) 
    { 
        path_ = s; 
    }
    void setPath(string&& s)
    {
        path_ = std::move(s);
    }

    string getPath() const { return path_; }

    void setReceiveTime(const Timestamp& t)
    {
        receiveTime_ = t;
    }
    Timestamp getReceiveTime() const
    {
        return receiveTime_;
    }

    bool addHeader(const char* begin, const char* end)
    {
        string line(begin, end);
        boost::trim(line);

        std::vector<string> result;
        boost::split(result, line, boost::is_any_of(": "), boost::token_compress_on);
        if(result.size() == 2)
        {
            headers_[result[0]] = result[1];
        }
        else
        {
            return false;
        }
        return true;
    }

    string getHeader(const string& field) const
    {
        string result;
        auto it = headers_.find(field);
        if(it != headers_.end())
        {
            result = it->second;
        }
        return result;
    }
    const std::map<string, string>& getHeaders() const
    {
        return headers_;
    }

    void swap(HttpRequest& that)
    {
        std::swap(method_, that.method_);
        std::swap(version_, that.version_);
        path_.swap(that.path_);
        query_.swap(that.query_);
        receiveTime_.swap(that.receiveTime_);
        headers_.swap(that.headers_);
    }

private:
    Method      method_;
    string      path_;
    Version     version_;
    string      query_;
    Timestamp   receiveTime_;
    std::map<string, string> headers_;
};

}