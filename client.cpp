#include "common.hpp"
#include "connection.hpp"
#include "sender.hpp"
#include "loop_runner.hpp"


int main(int argc, char** argv) {
    std::shared_ptr<Connection> conn = std::make_shared<Connection>();
    std::shared_ptr<Sender> sender = std::make_shared<Sender>();
    LoopRunner runner(2, sender);

    runner.run();
    printf("%d threads are running\n", runner.spawned());

    getchar();


    if(conn->connect("127.0.0.1", 8080)) {
        printf("Client not able to connect");
        return 0;
    }

    printf("Client connected successfully!\n");

    if(sender->enroll(conn)) printf("enrolled successfully!\n");

    while(true) {
        getchar();

        std::vector<uint8_t> bytes = {'a', 'b', 'c', 'd', 'e', 'f', 'g', '\n'};
        sender->send(conn, bytes, [] (Error error, std::vector<uint8_t> bytes, size_t size) {
            if(error == Error::Success) {
                printf("Sender did the job!\n");
                fflush(stdout);
            }
        });

    }

    


    return 0;
}