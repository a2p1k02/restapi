#include "restapi/restapi.h"

int main()
{
	routes_map routes;

	routes.insert({ "/hello", {{"status", 200}, {"message", "hello world"}} });
	routes.insert({ "/", {{"status", 200}, {"message", "you can route to /hello page"}} });

	restapi::m_routes = routes;
	restapi::start("127.0.0.1", 5734);

	return 0;
}
