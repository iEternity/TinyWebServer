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

    void setQuery(const char* start, const char* end) 
    { 
        query_ = string(start, end); 
    }
    string getQuery() const { return query_; }

    void setPath(const char* start, const char* end) 
    { 
        path_ = string(start, end); 
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

    void addHeader(const string& line)
    {
        boost::trim(line);

        std::vector<string> result;
        boost::split(result, line, boost::is_any_of(": "), boost::token_compress_on);
        headers_[result[0]] = result[1];
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

    static std::map<Method, string> methodString_ = 
    {
        {Method::kGet,      "GET"},
        {Method::kPost,     "POST"},
        {Method::kPut,      "PUT"},
        {Method::kDelete,   "DELETE"},
        {Method::kHead,     "HEAD"},
        {Method::kInvalid,  "UNKNOWN"}
    }
};

}