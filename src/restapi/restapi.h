#ifndef RESTAPI_H_
#define RESTAPI_H_

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio.hpp>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <string>

typedef unsigned short u16;

class restapi : public std::enable_shared_from_this<restapi> {
public: 
	explicit restapi(boost::asio::ip::tcp::socket socket);
	void run();
private: 
	boost::asio::ip::tcp::socket m_socket;
	boost::beast::flat_buffer m_buffer{ 8192 };
	boost::beast::http::request<boost::beast::http::dynamic_body> m_request;
	boost::beast::http::response<boost::beast::http::dynamic_body> m_response;

	boost::asio::steady_timer m_deadline = { m_socket.get_executor(), std::chrono::seconds(60)};

	static std::size_t request_count();
	static std::time_t now();

	void read_request();
	void process_request();
	void create_response();
	void write_response();
	void check_deadline();
};

#endif //RESTAPI_H_
