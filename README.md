# restapi

### this is a simple restapi lib for web development written with boost

## Examples
```cpp
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
```

## Logs example
![изображение](https://github.com/a2p1k02/restapi/assets/35633190/84f78a54-8fe9-4c92-a541-65b63b4c2f60)

