#include "asio.hpp"
#include "main.hpp"

#define BUFLEN 8192

io_context IOC;
char data_[BUFLEN];

static void routing_handler(string tag, mutable_buffer buf_,
                            function<void(mutable_buffer buf_)> cb);
static void out_handler(ip::udp::endpoint endpoint, mutable_buffer buf_,
                        function<void(mutable_buffer buf_)> cb);

class server {
public:
  server(string tag, string address, uint16_t port)
      : tag_(tag),
        socket_(IOC,
                ip::udp::endpoint(ip::address::from_string(address), port)) {
    do_receive();
  }

private:
  string tag_;
  ip::udp::socket socket_;
  ip::udp::endpoint sender_endpoint_;

  void do_receive() {
    cout << __FILE__ << ":" << __LINE__ << endl;

    socket_.async_receive_from(
        buffer(data_, BUFLEN), sender_endpoint_,
        [this](boost::system::error_code ec, size_t bytes_recvd) {
          cout << sender_endpoint_.address() << endl;
          cout << sender_endpoint_.port() << endl;

          if (!ec && bytes_recvd > 0) {
            cout << __FILE__ << ":" << __LINE__ << endl;
            routing_handler(tag_, buffer(data_, bytes_recvd),
                            [this](mutable_buffer buf_) { do_send(buf_); });
          } else {
            cout << ec.message() << endl;
            abort();
            do_receive();
          }
        });
  }

  void do_send(mutable_buffer buf_) {
    cout << __FILE__ << ":" << __LINE__ << endl;

    cout << sender_endpoint_.address() << endl;
    cout << sender_endpoint_.port() << endl;

    socket_.async_send_to(
        buf_, sender_endpoint_,
        [this](boost::system::error_code, size_t) { do_receive(); });
  }
};

void do_listen(ptree root) {
  string tag;
  string address;
  uint16_t port;

  if (root.empty()) {
    root.push_back(make_pair("", ptree{}));
  }

  for (auto &in : root) {
    tag = in.second.get("tag", "");
    address = in.second.get("address", "0.0.0.0");
    port = in.second.get("port", 1053);
    new server(tag, address, port);
  }

  cout << "IOC.run();" << endl;
  IOC.run();
}

static void routing_handler(string tag, mutable_buffer buf_,
                            function<void(mutable_buffer buf_)> cb) {
  cout << __FILE__ << ":" << __LINE__ << endl;

  size_t i, ir, io; /* index routing, index out */
  char *p1, *p2;    /* p1 for buffer, p2 for domain_ */
  static char domain_[128];

  /* match in tag */
  for (ir = 0; ir < routing_head.size(); ir++) {
    if (routing_head[ir].in_tag.empty()) {
      goto match_domain;
    } else {
      if (tag.empty()) {
        continue;
      } else {
        for (auto &it : routing_head[ir].in_tag) {
          if (tag == it) {
            goto match_domain;
          }
        }
      }
    }
  }

match_domain:
  p1 = (char *)buf_.data() + 12;
  p2 = domain_;
  while (*p1 != 0) {
    for (i = 0; i < (uint8_t)*p1; i++) {
      *(p2 + i) = *(p1 + i + 1);
    }
    p1 += (i + 1);
    *(p2 + i) = '.';
    p2 += (i + 1);
  }
  *(p2 - 1) = '\0';

  for (; ir < routing_head.size(); ir++) {
    if (routing_head[ir].domain.empty()) {
      goto match_out;
    } else {
      for (auto &rd : routing_head[ir].domain) {
        if (regex_search(domain_, regex(rd))) {
          goto match_out;
        }
      }
    }
  }

match_out:
  if (ir == routing_head.size() || routing_head[ir].out_tag.empty()) {
    io = 0;
  } else {
    for (io = 0; io < out_head.size(); io++) {
      if (routing_head[ir].out_tag == out_head[io].tag) {
        break;
      }
    }
    assert(io < out_head.size());
  }

  cout << out_head[io].tag << endl;
  out_handler(ip::udp::endpoint(ip::address::from_string(out_head[io].address),
                                out_head[io].port),
              buf_, cb);
}

static void out_handler(ip::udp::endpoint endpoint, mutable_buffer buf_,
                        function<void(mutable_buffer buf_)> cb) {
  cout << __FILE__ << ":" << __LINE__ << endl;

  shared_ptr<ip::udp::socket> socket_(
      new ip::udp::socket(IOC, ip::udp::endpoint(ip::udp::v4(), 0)));

  (*socket_).async_send_to(
      buf_, endpoint, [socket_, cb](boost::system::error_code ec, size_t) {
        cout << __FILE__ << ":" << __LINE__ << endl;

        (*socket_).async_receive(
            buffer(data_, BUFLEN),
            [socket_, cb](boost::system::error_code ec, size_t bytes_recvd) {
              cout << __FILE__ << ":" << __LINE__ << endl;

              cb(buffer(data_, bytes_recvd));
            });
      });
}
