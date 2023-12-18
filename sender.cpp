#include "sender.hpp"

Outbox* Sender::findOutbox(int fd) {
    auto it = map.find(fd);
    if(it == map.end()) {
        return nullptr;
    }else {
        return it->second;
    }
}

bool Sender::enroll(std::shared_ptr<Connection> connection) {
    std::lock_guard<std::mutex> lock(mutex);

    if(connection == nullptr) return false;
    if(!connection->isConnected()) return false;

    int fd = Connection::Handle::get(*connection);

    Outbox* outbox = findOutbox(fd);
    if(outbox != nullptr) {
        outbox->unenroll = false;
        return true;
    }

    outbox = new Outbox;
    outbox->connection = std::move(connection);

    map[fd] = outbox;
    add(fd, FLAGS_ACTIVE, outbox);
    return true;
}

bool Sender::send(std::shared_ptr<Connection> connection, std::vector<uint8_t> bytes, Callback callback) {
    std::lock_guard<std::mutex> lock(mutex);

    if(connection == nullptr) return false;
    if(!connection->isConnected()) return false;
    if(!callback) return false;

    int fd = Connection::Handle::get(*connection);

    Outbox* outbox = findOutbox(fd);
    if(outbox == nullptr) return false;
    if(outbox->unenroll) return false;

    int size = outbox->packets.enqueue({std::move(bytes), std::move(callback)});    
    if(size == -1) {
        printf("WAS EMPTY AND IS NOT NOW!\n");
        fflush(stdout);
        mod(fd, FLAGS_ACTIVE, outbox);
    }

    return true;
}

bool Sender::unenroll(std::shared_ptr<Connection> connection) {
    std::lock_guard<std::mutex> lock(mutex);

    if(connection == nullptr) return false;
    int fd = Connection::Handle::get(*connection);

    Outbox* outbox = findOutbox(fd);
    if(outbox == nullptr) return false;

    outbox->unenroll = true;
    return true;
}


void Sender::handleEvents(uint32_t events, void* data) {
    Outbox* outbox = static_cast<Outbox*>(data);
    Error error;
    if(events & EPOLLERR) {
        error = Error::Fatal;
    }
    else if(events & EPOLLHUP) {
        error =  Error::Shutdown;
    } 
    else {
        error = dispatch(outbox);
    }
    handleError(error, outbox);
}


Error Sender::dispatch(Outbox* outbox) {
    auto& [connection, unenroll, packets, packet, dispatcher] = *outbox;
    int fd = Connection::Handle::get(*(connection));

    while(true) {
        if(unenroll) return Error::Interrupt;

        if(!dispatcher.isLoaded()) {
            if(!packets.tryDequeue(packet)) return Error::NoBytes;
            dispatcher.load(fd, packet.bytes);
        }

        int sent = dispatcher.send();

        if(sent >= 0) {
            if(!dispatcher.isDone()) continue;

            dispatcher.discard();
            return Error::Success;
        }
        if (errno == EINTR) continue;
        if(errno == EWOULDBLOCK || errno == EAGAIN) return Error::WouldBlock;
        if(errno == EPIPE) return Error::Shutdown;
        return Error::Fatal;
    }
}


void Sender::handleError(Error error, Outbox* outbox) {
    int fd = Connection::Handle::get(*(outbox->connection));
    switch(error) {
        case Error::Success:
            onSuccess(outbox);
            break;
        case Error::WouldBlock:
            mod(fd, FLAGS_ACTIVE, outbox);
            break;
        case Error::NoBytes:
            mod(fd, FLAGS_INACTIVE, outbox);
            break;
        default:
            onFailure(error, outbox);
    }
}

void Sender::onSuccess(Outbox* outbox) {
    int fd = Connection::Handle::get(*(outbox->connection));
    Packet packet = std::move(outbox->packet);
    size_t sent = packet.bytes.size();

    mod(fd, FLAGS_ACTIVE, outbox);

    packet.callback(Error::Success, std::move(packet.bytes), sent);
}

void Sender::onFailure(Error error, Outbox* outbox) {
    int fd = Connection::Handle::get(*(outbox->connection));
    Packet packet = std::move(outbox->packet);
    size_t sent = outbox->dispatcher.bytesSent();

    {
        std::lock_guard<std::mutex> lock(mutex);
        if(error == Error::Interrupt && !outbox->unenroll) {
            mod(fd, FLAGS_ACTIVE, outbox);
            return;
        }
        del(fd);
        delete outbox;
        map.erase(fd);
    }

    if(packet.callback) packet.callback(error, std::move(packet.bytes), sent);
}
