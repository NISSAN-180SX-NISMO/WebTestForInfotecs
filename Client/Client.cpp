// App 1
#pragma warning(disable: 4996)
#pragma comment(lib, "Ws2_32.lib")
#include <WinSock2.h>
#include <iostream>
#include <thread>
#include "../packages/StringHandler/Converter.h"
#include "../packages/StringHandler/CalculatedShow.h"
#include "../packages/StringHandler/StringHandlerExceptions.h"


void consoleInput(std::string & buffer) {
    std::cout << "For exit input \"exit\"\nInput your string > ";
    std::getline(std::cin, buffer);
}

int main()
{
    setlocale(LC_ALL, "");
    

    WSAData wsaData;
    if (WSAStartup(MAKEWORD(2, 1), &wsaData)) {
        std::cout << "ERROR!" << std::endl;
        return 1;
    }

    SOCKADDR_IN address;
    int addressSize = sizeof(address);
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    address.sin_port = htons(1111);
    address.sin_family = AF_INET;

    while (true) {
        SOCKET socket_connection = socket(AF_INET, SOCK_STREAM, NULL);
        if (connect(socket_connection, (SOCKADDR*)&address, addressSize)) {
            std::cout << "ERROR! FAILED CONNECT TO SERVER!" << std::endl;
            return 1;
        }
        std::cout << "success connect to server!" << std::endl;

        std::string buffer;

        StringHandler* converter;
        StringHandler* calculator;
            std::thread thConverter([&]() {
                consoleInput(buffer);
                if (buffer == "exit") {
                    send(socket_connection, (buffer + '\0').c_str(), sizeof((buffer + '\0')), NULL);
                    exit(0);
                }
        try {
                converter = new Converter(buffer);
                buffer = converter->get();
        }
        catch (const SizeOverflow& ex) { std::cout << ex.what() << " " << ex.getMore() << std::endl; buffer.clear(); }
        catch (const InvalidChar& ex) { std::cout << ex.what() << " \"" << ex.getMore() << "\"" << std::endl; buffer.clear(); }
                });
            thConverter.join();
            if (buffer == "") continue;
            std::thread thCalculator([&]() {
                calculator = new CalculatedShow(buffer);
                buffer = calculator->get();
                });
            thCalculator.join();

            send(socket_connection, (buffer + '\0').c_str(), sizeof((buffer + '\0')), NULL);
            std::cout << "message sended!" << std::endl;

    }

}