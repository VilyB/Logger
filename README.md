# ImGui TCP Log Viewer

A tiny SDL2 + ImGui app that listens on 127.0.0.1:8000 for JSON log messages, shows them in a table, and writes them to disk.

## Scope
- Application can run on Windows & Ubuntu
- Open a TCP socket on 127.0.0.1:8000
- Receive JSON messages from external clients through TCP socket
- Display messages in a GUI
- Place messages in a log file
- Handles only 1 client at a time
- Language: Modern C++
- UI Library/Framework: SDL2 + ImGui
- Build System: Make

Each log message has this shape:
```
{
  "timestamp": "123124214",
  "app": "app_name",
  "message": "my message"
}
```
Messages are sent one at a time over a persistent TCP connection and are newline-delimited
(NDJSON). Example line below:
```
{"timestamp":"123124214","app":"app_name","message":"my message"}\n
```
## Dependencies

- C++17
- SDL2 (added as a submodule)
- ImGui (added as a submodule)

### Ubuntu/Debian
```
sudo apt update
sudo apt install build-essential pkg-config libsdl2-dev
```
Then pull the ImGui submodule
```
git submodule update --init --recursive
```
## Build
```
make
```
## Run
```
make run
```
## Testing
```
echo '{"timestamp":"1699999999","app":"demo","message":"hello"}' | nc 127.0.0.1 8000
```
## Alternative Testing

A client was written to test the server by running a producer thread tasked to send
new messages at a 250ms interval to the 127.0.0.1:8000 host and port.

## Build & Test Client
```
g++ client.cpp
./a.out
```
## Modifications
- Bind address/port: defaults to 127.0.0.1:8000 and is hardcoded in TcpLogServer.h
- Log Capacity: adjust the LogBuffer capacity in main.cpp (e.g. LogBuffer buffer(1000);)
- Set the filepath: change the file path for the logs in FileLogger.h
