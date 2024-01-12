#ifndef RESTAPI_H_
#define RESTAPI_H_

#ifdef _WIN32
#  define BOOST_JSON_NO_LIB
#  define BOOST_CONTAINER_NO_LIB
#endif // _WIN32

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio.hpp>
#include <boost/json.hpp>
#include <boost/foreach.hpp>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <string>
#include <map>

typedef unsigned short u16;
typedef std::map<std::string, boost::json::object> routes_map;

class restapi : public std::enable_shared_from_this<restapi> {
public: 
	static routes_map m_routes;

	explicit restapi(boost::asio::ip::tcp::socket socket);
	static void start(const char* host, const u16 port);
	static void connect(boost::asio::ip::tcp::acceptor& acceptor, boost::asio::ip::tcp::socket& socket);
	void run();
private: 
	boost::asio::ip::tcp::socket m_socket;
	boost::beast::flat_buffer m_buffer{ 8192 };
	boost::beast::http::request<boost::beast::http::dynamic_body> m_request;
	boost::beast::http::response<boost::beast::http::dynamic_body> m_response;

	boost::asio::steady_timer m_deadline = { m_socket.get_executor(), std::chrono::seconds(60)};

	void log();
	static const char* get_time();

	void read_request();
	void process_request();
	void create_response(const std::string& path);
	void write_response();
	void check_deadline();
};

#endif //RESTAPI_H_
