#include "common.hpp"
// #include "tsqueue.hpp"
#include "header.hpp"
#include "message.hpp"
// #include "async_epoll.hpp"
// #include "sender.hpp"
#include "event_loop.hpp"
#include "listener.hpp"

#define PRESS() {printf("\npress to continue\n"); getchar();}

enum Request {
    Greeting = 50,
    Farewell = 51,
};  

// void* routine(void* loopPtr) {
//     EventLoop* loop = (EventLoop*) loopPtr;
//     loop->join();
//     return nullptr;
// }

int main(int argc, char** argv) {
    // Message<Request> message(Request::Greeting);
    // std::vector<uint8_t> s = Header<Request>::serialize(message.getHeader());
    // printf("haha %zu %zu", s.size(), sizeof(Header<Request>));
    // EventLoop eventLoop;
    // pthread_t threads[3];
    
    // pthread_create(&threads[0], nullptr, &routine, &eventLoop);
    // pthread_create(&threads[1], nullptr, &routine, &eventLoop);

    
    // eventLoop.release();

    // pthread_create(&threads[2], nullptr, &routine, &eventLoop);

    // pthread_join(threads[0], nullptr);
    // pthread_join(threads[1], nullptr);

    // eventLoop.release();

    // pthread_join(threads[2], nullptr);

    // printf("Done");

    // Listener listener;
    // if(listener.bind("127.0.0.1", 8080)) {
    //     printf(" successfully bound\n");
    // }else {
    //     printf(" failed to bound\n");
    //     return 0;
    // }

    // if(listener.listen(1)) {
    //     printf(" successfully listening\n");
    // }else {
    //     printf(" failed to listen\n");
    //     return 0;
    // }  

    // std::shared_ptr<Connection> clientConn = listener.accept();
    // if(clientConn == nullptr){
    //     printf(" failed to accept\n");
    //     return 0;
    // } else {
    //     printf(" successfully accepted\n");
    // }

    // printf(" %s", clientConn->getIp().c_str());
    // printf(" %d", clientConn->getPort());

    // getchar();

    // Message<Request> message;
    // message.type = Farewell;
    // printf("Size of Message: %zu", message.size());

    // PRESS();

    // int someInt = 0x61616161;
    // int anotherInt = 0x62626262;

    // struct Human{
    //     int color;
    //     char height;
    //     short age;
    // };
    // Human mary = { 0x63636363,0x64, 0x6666 };
    // message << mary << mary << mary << mary;

    // std::vector<uint8_t> raw = serialize(message);

    // write(1, raw.data(), raw.size());

    // PRESS();

    // Message<Request> phoenix;
    // if(deserialize(raw, phoenix)) printf("SUCCESSFULLY DESERIALIZED!\n");

    // printf("size: %zu\n", phoenix.size());


    // PRESS();

    // Human eva;
    // phoenix >> eva >> eva >> eva >> eva;

    // printf("%d %d %d", eva.color, eva.height, eva.age);

    // printf("size: %zu", phoenix.size());

    // const char buf[5] = {'K', 'A', 'I', 'E', '\0'};


    // int ret = send(1, buf, sizeof(buf), 0);


    // printf("%d reeeturn ", ret);



    // PRESS();

    // printf("\n okay let's try other way around\n");


    // printf("Size of Message: %zu\n", message.size());

}