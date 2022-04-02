#ifndef NETCAT_CONNECTION_H
#define NETCAT_CONNECTION_H

class Connection {
public:
    virtual ~Connection() = default;
    virtual void inRead() = 0;
    virtual void inWrite() = 0;
    virtual void outRead() = 0;
    virtual void outWrite() = 0;
};

// TODO: implement ConnWithLogger
// class ConnWithLogger : public Connection {
// public:
//     ConnWithLogger(Logger *logger, Connection *conn);
//
//     void inRead() override;
//     void inWrite() override;
//     void outRead() override;
//     void outWrite() override;
//
// private:
//     Logger *_logger;
// };


#endif //NETCAT_CONNECTION_H
