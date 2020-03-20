#include "cache.hh"
#include "lru_evictor.hh"
#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <optional>
#include <boost/asio.hpp>
#include <nlohmann/json.hpp>

// for convenience
using json = nlohmann::json;

using size_type = uint64_t;

bool is_number(std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

bool is_ip(std::string& s)
{
    if (*s.begin() == '.' && *(s.end() - 1) == '.') {return false;}
    std::string::const_iterator it = s.begin();
    auto no_consec_dots = true;
    while (it != s.end() && (std::isdigit(*it) || (*it) == '.') && no_consec_dots) 
    {
        ++it;
        no_consec_dots = (*it == '.' && *(it - 1) != '.') || (*it) != '.';
    }
    return !s.empty() && it == s.end();
}

size_type to_number(std::string& s)
{
    return size_type(std::stoi(s));
}

bool set_params(size_type& maxmem, std::string& server, size_type& port, size_type& threads, int argc, char** argv)
{
    char opt;
    while ((opt = getopt(argc, argv, "m:p:s:t:")) != -1)
    {
        std::string param_str(optarg);

        switch(opt)
        {
            case 'm':
                if (is_number(param_str)) 
                {
                    auto param_uint = to_number(param_str);
                    maxmem = param_uint;
                }
                else
                {
                    std::cout << "Request failed: invalid parameter format for -m option" << std::endl;
                    return false;
                }
                break;
            
            case 'p':
                if (is_number(param_str)) 
                {
                    auto param_uint = to_number(param_str);
                    port = param_uint;
                }
                else
                {
                    std::cout << "Request failed: invalid parameter format for -p option" << std::endl;
                    return false;
                }
                break;

            case 's':
                if (is_ip(param_str))
                {
                    server = param_str;
                }
                else
                {
                    std::cout << "Request failed: invalid parameter format for -s option" << std::endl;
                    return false;
                }
                break;

            case 't':
                if (is_number(param_str)) 
                {
                    auto param_uint = to_number(param_str);
                    threads = param_uint;
                }
                else
                {
                    std::cout << "Request failed: invalid parameter format for -t option" << std::endl;
                    return false;
                }
                break;  
            
            case '?':
                std::cout << "Request failed: undefined option" << std::endl;
                return false;
        }

    }

    return true;
}

class session : public std::enable_shared_from_this<session>
{
public:

    session(boost::asio::ip::tcp::socket&& socket, Cache* cache)
    : socket(std::move(socket)), cache_(cache)
    {
    }

    void start()
    {
        boost::asio::async_read_until(socket, streambuf, "\n", [self = shared_from_this()] (boost::system::error_code error, std::size_t bytes_transferred)
        {
            std::string req( (std::istreambuf_iterator<char>(&self->streambuf)), std::istreambuf_iterator<char>() );
            json req_j = json::parse(req);
            
            bool is_valid_req = true;
            parse_request(req, is_valid_req);

            if (is_valid_req)
            {
                
            }
            boost::asio::async_write(self->socket, boost::asio::buffer("message received\n"), [self = self->shared_from_this()] (boost::system::error_code error, std::size_t bytes_transferred)
            {
                std::cout << "handled" << std::endl;
            });
        });
    }

private:

    boost::asio::ip::tcp::socket socket;
    boost::asio::streambuf streambuf;
    Cache* cache_;

    std::string parse_request(std::string req, bool& is_valid)
    {
        std::string response("");
        std::vector<std::string> tokens;
        size_t delim_pos; = req.find("/");
        if (delim_pos != std::string::npos) {tokens.push_back(req)}
        else
        {
            while(delim_pos != std::string::npos)
            {
                tokens.push_back( (req.substr(0, delim_pos)) );
                req = req.substr(delim_pos);
            }
        }

        if (tokens.size() < 1 || tokens.size() > 3)
        {
            is_valid = false;
            return "";
        }

        if (tokens[0] == "HEAD")
        {
            response += "Space-Used: ";
            response += std::to_string(cache_->space_used());
            response += "\nHTTP Version: 1.1\n";
            response += "Accept: text/html\n";
            response += "Content Type: application/json\n";
        }
        else if(tokens[0] == "GET")
        {
            if (tokens.size() != 2) {is_valid = false; return "";}
            else
            {
                json tuple;
                auto val = cache_->get(tokens[1]);
                if (val == nullptr) {return "error: 501";}
                tuple["key"] = tokens[1];
                tuple["value"] = cache_->get(tokens[1]);
                is_valid = false;
                return tuple.dump();
            }
        }
        else if(tokens[0] == "PUT")
        {
            if (tokens.size() != 3) {is_valid = false; return "";}
            else
            {
                cache_->set(tokens[1], tokens[2]);
                is_valid = true;
                return "";
            }
        }
        else if(tokens[0] == "DELETE")
        {
            if (tokens.size() != 2) {is_valid = false; return "";}
            else
            {
                cache_->del(tokens[1]);
                is_valid = true;
                return "";
            }
        }

        else if (tokens[0] == "POST")
        {
            if (tokens.size() != 2) {is_valid = false; return "";}
            else
            {
                if (tokens[1] != "reset") {is_valid = false; return "";}
                else
                {
                    cache_->reset();
                    is_valid = true;
                    return "";
                }
            }
        }
    }


};

class server
{
public:

    server(boost::asio::io_context& io_context, std::string address, std::uint64_t port, Cache* cache)
    : io_context(io_context)
    , acceptor  (io_context, boost::asio::ip::tcp::endpoint(ip::address_v4::make_address_v4(address), port))
    , cache_ (cache)
    {
    }

    void async_accept()
    {
        socket.emplace(io_context);

        acceptor.async_accept(*socket, [&] (boost::system::error_code error)
        {
            std::make_shared<session>(std::move(*socket), cache_)->start();
            async_accept();
        });
    }

private:

    boost::asio::io_context& io_context;
    boost::asio::ip::tcp::acceptor acceptor;
    std::optional<boost::asio::ip::tcp::socket> socket;
    Cache* cache_;
};

int main(int argc, char** argv)
{
    size_type maxmem = 128;
    std::string address("127.0.0.1");
    size_type port = 10002;
    size_type threads = 1;

    auto request_success = set_params(maxmem, address, port, threads, argc, argv);
    if (!request_success) {return -1;}

    std::cout << std::endl;
    std::cout << "maxmem: " << maxmem << std::endl;
    std::cout << "server address: " << address << std::endl;
    std::cout << "port: " << port << std::endl;
    std::cout << "threads: " << threads << std::endl;

    LRU_Evictor* evictor = new LRU_Evictor();
    Cache* cache = new Cache(maxmem, 0.75, evictor);

    boost::asio::io_context io_context;
    server srv(io_context, address, port, cache);
    srv.async_accept();
    io_context.run();
    return 0;
    
}