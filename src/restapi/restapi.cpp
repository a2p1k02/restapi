#include "restapi.h"

restapi::restapi(boost::asio::ip::tcp::socket socket) : m_socket(std::move(socket)) {}

void restapi::run()
{
    this->read_request();
    std::time_t connected_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::cout
        << std::ctime(&connected_time)
        << m_socket.remote_endpoint().address().to_string()
        << ":"
        << m_socket.remote_endpoint().port()
        << std::endl;
    this->check_deadline();
}

std::size_t restapi::request_count()
{
    static std::size_t count = 0;
    return ++count;
}

std::time_t restapi::now()
{
    return std::time(0);
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
            this->create_response();
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

void restapi::create_response()
{
    if (m_request.target() == "/count") {
        m_response.set(boost::beast::http::field::content_type, "text/html");
        boost::beast::ostream(m_response.body())
                << "<html>\n"
                <<  "<head><title>Request count</title></head>\n"
                <<  "<body>\n"
                <<  "<h1>Request count</h1>\n"
                <<  "<p>There have been "
                <<  restapi::request_count()
                <<  " requests so far.</p>\n"
                <<  "</body>\n"
                <<  "</html>\n";
    } else if (m_request.target() == "/time") {
        m_response.set(boost::beast::http::field::content_type, "text/html");
        boost::beast::ostream(m_response.body())
                << "<html>\n"
                <<  "<head><title>Current time</title></head>\n"
                <<  "<body>\n"
                <<  "<h1>Current time</h1>\n"
                <<  "<p>The current time is "
                <<  restapi::now()
                <<  " seconds since the epoch.</p>\n"
                <<  "</body>\n"
                <<  "</html>\n";
    } else {
        m_response.result(boost::beast::http::status::not_found);
        m_response.set(boost::beast::http::field::content_type, "text/plain");
        boost::beast::ostream(m_response.body()) << "Error 404\n\nFile not found\n";
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
