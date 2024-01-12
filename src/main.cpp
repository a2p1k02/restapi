#include "restapi/restapi.h"

int main()
{
	routes_map routes;

	routes.insert({ "/hello", {{"status", 200}, {"message", "hello world"}} });
	routes.insert({ "/test", {{"status", 200}, {"message", "hello from test page"}, {"location", "Moscow"}}});
	routes.insert({ "/", {{"status", 200}, {"message", "you can route to /hello page"}} });

	restapi::m_routes = routes;
	restapi::start("127.0.0.1", 1515);

	return 0;
}
