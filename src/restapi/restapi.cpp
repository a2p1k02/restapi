#include "restapi.h"
#include <boost/json/src.hpp>

routes_map restapi::m_routes{};

restapi::restapi(boost::asio::ip::tcp::socket socket) : m_socket(std::move(socket)) {}

void restapi::run()
{
    this->read_request();
    this->log();
    this->check_deadline();
}

void restapi::connect(boost::asio::ip::tcp::acceptor& acceptor, boost::asio::ip::tcp::socket& socket)
{
    acceptor.async_accept(socket,
    [&](boost::beast::error_code ec) {
        if (!ec) {
          auto app = std::make_shared<restapi>(std::move(socket));
          app->run();
        }
        connect(acceptor, socket);
    });
}

void restapi::start(const char* host, const u16 port)
{
    try {

        std::cout << "[" << get_time() << "] " << "Connecting...\n";

        boost::asio::io_context ioc(1);
        boost::asio::ip::tcp::acceptor acceptor(ioc, { boost::asio::ip::make_address(host), port });
        boost::asio::ip::tcp::socket socket(ioc);

        restapi::connect(acceptor, socket);

        std::cout <<"[" << get_time() << "] " << "Connected!\n";

        std::cout << "Available routes: ";
        BOOST_FOREACH(const auto & key, m_routes) {
            std::cout
                << key.first << " ";
        }
        std::cout << "\n";

        ioc.run();
    }
    catch (std::exception const& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return;
    }
}

const char* restapi::get_time()
{
    std::time_t connected_time;
    char* time;

    connected_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    time = std::ctime(&connected_time);
    time[strlen(time) - 1] = '\0';

    return time;
}

void restapi::log()
{
    std::cout
        << "[" << get_time() << "] "
        << m_socket.remote_endpoint().address().to_string()
        << ":"
        << m_socket.remote_endpoint().port();
}

void restapi::read_request()
{
    std::shared_ptr<restapi> self = shared_from_this();
    boost::beast::http::async_read(
            m_socket, m_buffer, m_request, [self](boost::beast::error_code ec, std::size_t bytes_trans) {
                boost::ignore_unused(bytes_trans);
                if (!ec) self->process_request();
            });
}

void restapi::process_request()
{
    m_response.version(m_request.version());
    m_response.keep_alive(false);

    switch (m_request.method()) {
        case boost::beast::http::verb::get:
            m_response.result(boost::beast::http::status::ok);
            m_response.set(boost::beast::http::field::server, "Beast");
            !m_routes[m_request.target()].empty() ? this->create_response(m_request.target()) : this->create_response("");
            break;
        default:
            m_response.result(boost::beast::http::status::bad_request);
            m_response.set(boost::beast::http::field::content_type, "text/plain");
            boost::beast::ostream(m_response.body())
                    << "Invalid request method '"
                    << std::string(m_request.method_string())
                    << "'";
            break;
    }

    this->write_response();
}

void restapi::create_response(const std::string& path)
{
    if (m_request.target() == path) {
        std::cout << " - HTTP/1.1 200 OK \n";
        m_response.set(boost::beast::http::field::content_type, "application/json");
        boost::beast::ostream(m_response.body())
            << m_routes[path];
    } else {
        m_response.result(boost::beast::http::status::not_found);
        std::cout << " - HTTP/1.1 404 NOT FOUND \n";
        m_response.set(boost::beast::http::field::content_type, "application/json");
        boost::beast::ostream(m_response.body()) << boost::json::object({ { "message", "404 not found" } });
    }
}

void restapi::write_response()
{
    std::shared_ptr<restapi> self = shared_from_this();

    m_response.content_length(m_response.body().size());

    boost::beast::http::async_write(
            m_socket,
            m_response,
            [self](boost::beast::error_code ec, std::size_t) {
                self->m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
                self->m_deadline.cancel();
            });

}

void restapi::check_deadline()
{
    std::shared_ptr<restapi> self = shared_from_this();

    m_deadline.async_wait([self](boost::beast::error_code ec) {
        if (!ec) self->m_socket.close(ec);
    });
}
