//
// Created by zhangkuo on 17-11-18.
//

#ifndef XNET_ZLIBSTREAM_H
#define XNET_ZLIBSTREAM_H
#include <zlib.h>
#include <xnet/base/noncopyable.h>
#include <xnet/net/Buffer.h>
#include <xnet/base/StringPiece.h>

namespace xnet
{

class ZlibInputStream : noncopyable
{
public:
    explicit ZlibInputStream(Buffer* output):
        output_(output),
        zerror_(Z_OK),
        bufferSize_(1024)
    {
        ::bzero(zstream_, sizeof zstream_);
        zerror_ = inflateInit(&zstream);
    }

    ~ZlibInputStream()
    {
        finish();
    }

    bool finish()
    {
        if(zerror_ != Z_OK) return false;

        while(zerror_ == Z_OK)
        {
            zerror_ = decompress(Z_FINISH);
        }

        zerror_ = inflateEnd(&zstream_);
        bool ok = (zerror_ == Z_OK);
        zerror_ = Z_STREAM_END;

        return ok;
    }

    bool write(const StringPiece& buf)
    {
        if(zerror_ != Z_OK) return false;

        void* in = const_cast<char*>(buf.data());
        zstream_.next_in = static_cast<Bytef*>(in);
        zstream_.avail_in = buf.size();

        while(zstream_.avail_in > 0 && zerror_ == Z_OK)
        {
            zerror_ = decompress(Z_NO_FLUSH);
        }

        if(zstream_.avail_in == 0) zstream_.next_in = nullptr;

        return zerror_ == Z_OK;
    }

    bool write(Buffer* input)
    {
        if(zerror_ != Z_OK) return false;

        void* in = const_cast<char*>(input->peek());
        zstream_.next_in = in;
        zstream_.avail_in = input->readableBytes();

        if(zstream_.avail_in > 0 && zerror_ == Z_OK)
        {
            zerror_ = decompress(Z_NO_FLUSH);
        }

        input->retrieve(input->readableBytes() - zstream_.avail_in);
        return zerror_ == Z_OK;
    }
private:
    int decompress(int flush)
    {
        output_->ensureWritableBytes(bufferSize_);
        zstream_->avail_out = output_->writableBytes();
        zstream_->next_out = static_cast<Bytef*>(output_->beginWrite());

        int error = inflate(&zstream_, flush);
        output_->hasWriten(output_->writableBytes() - zstream_ .avail_out);

        if(output_->writableBytes() == 0 && bufferSize_ <= 64*1024)
        {
            bufferSize *= 2;
        }

        return error;
    }

private:
    Buffer*     output_;
    z_stream    zstream_;
    int         zerror_;
    int         bufferSize_;
};

class ZlibOutputStream : noncopyable
{
public:
    explicit ZlibOutputStream(Buffer* output):
        output_(output),
        zerror_(Z_OK),
        bufferSize_(1024)
    {
        ::bzero(&zstream_, sizeof zstream_);
        zerror_ = deflateInit(&zstream_, Z_DEFAULT_COMPRESSION);
    }

    ~ZlibOutputStream()
    {
        finish();
    }

    bool finish()
    {
        if(zerror_ != Z_OK) return false;

        while(zerror_ == Z_OK)
        {
            zerror_ = compress(Z_FINISH);
        }
        zerror_ = deflateEnd(&zstream_);
        bool ok = (zerror_ == Z_OK);
        zerror_ = Z_STREAM_END;
        return ok;
    }

    const char* zlibErrorMessage() const    { return zstream_.msg; }
    int         zlibErrorCode() const       { return zerror_; }
    int64_t     inputBytes() const          { return zstream_.total_in; }
    int64_t     outputBytes() const         { return zstream_.total_out; }
    int         bufferSize() const          { return bufferSize_; }

    bool write(const StringPiece& buf)
    {
        if(zerror_ != Z_OK) return false;

        assert(zstream_.avail_in == 0 && zstream_.next_in == nullptr);

        void* in = const_cast<char*>(buf.data());
        zstream_.next_in = static_cast<Bytef*>(in);
        zstream_.avail_in = buf.size();

        while(zstream_.avail_in > 0 && zerror_ == Z_OK)
        {
            zerror_ = compress(Z_DEFAULT_COMPRESSION);
        }

        if(zstream_.avail_in == 0) zstream_.next_in = nullptr;

        return zerror_ == Z_OK;
    }

    bool write(Buffer* input)
    {
        if(zerror_ != Z_OK) return false;

        void* in = static_cast<char*>(input->peek());
        zstream_.next_in = in;
        zstream_.avail_in = input->readableBytes();
        if(zstream_.avail_in > 0 && zerror_ == Z_OK)
        {
            zerror_ = compress(Z_NO_FLUSH);
        }

        input->retrieve(input->readableBytes() - zstream_->avail_in);
        return zerror_ == Z_OK;
    }

private:
    int compress(int flush)
    {
        output_->ensureWritableBytes(bufferSize_);
        zstream_.next_out = static_cast<Bytef*>(output_->beginWrite());
        zstream_.avail_out = static_cast<unsigned int>(output_->writableBytes());
        int error = ::deflate(&zstream, flush);
        output_->hasWriten(output_->writableBytes() - zstream_.avail_out);
        if(output_->writableBytes() == 0 && bufferSize_ < 65536)
        {
            bufferSize_ *= 2;
        }
        return error;
    }

private:
    Buffer*     output_;
    z_stream    zstream_;
    int         zerror_;
    int         bufferSize_;
};

}

#endif //XNET_ZLIBSTREAM_H
