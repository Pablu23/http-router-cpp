# Http Router which is supposed to be similar to the standard Golang Router
## Dependencies
- c++20
- Linux
- make for building
- gcc (g++) if using make for build

## How to build
Run Example server implemented in main.cpp
```
make run
```

Build static library
```
make
```

Install library and header files (Possibly unsafe, could overwrite existing headers or libraries)
```
sudo make install
```

Remove installed library and header files (Also unsafe for the same reasons)
```
sudo make remove
```

## Todo list
Todos: 
- Routing to Tree instead of map
- Cmake maybe?
- Make redirects available
- Add windows support
- Stop
- React to Method
- Implement actual http protocol
- Post Form
