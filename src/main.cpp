#include "restapi/restapi.h"

void connect(boost::asio::ip::tcp::acceptor& acceptor, boost::asio::ip::tcp::socket& socket)
{
    acceptor.async_accept(socket,
        [&](boost::beast::error_code ec) {
            if (!ec) std::make_shared<restapi>(std::move(socket))->run();
            connect(acceptor, socket);
        });
}

int main()
{
    try {
        boost::asio::ip::address const address = boost::asio::ip::make_address("127.0.0.1");
        u16 port = 8080;

        std::cout << "Connecting!\n";

        boost::asio::io_context ioc(1);
        boost::asio::ip::tcp::acceptor acceptor(ioc, {address, port});
        boost::asio::ip::tcp::socket socket(ioc);

        connect(acceptor, socket);

        std::cout << "Connected!\n";
        ioc.run();

    } catch (std::exception const& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return 0;
}
