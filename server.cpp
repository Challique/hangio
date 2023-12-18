#include "common.hpp"
#include "connection.hpp"
#include "listener.hpp"



int main(int argc, char** argv) {
    Listener listener;
    listener.bind("127.0.0.1", 8080);
    listener.listen(1);
    std::unique_ptr<Connection> conn = listener.accept();
    if(conn == nullptr) {
        printf("Server Failed to establish connection!\n");
        return 0;
    }

    printf("Server established Connection!\n");

    while(true) {
        std::vector<uint8_t> bytes;

        int received = conn->recv(bytes, 100);
        if(received == -1) {
            printf("Some error occured!\n");
        }else {
            printf("Received %d bytes: ", received);
            fflush(stdout);
            write(1, bytes.data(), bytes.size());
        }

        sleep(1);


    }




    return 0;
}