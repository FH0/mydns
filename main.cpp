#include "main.hpp"
#include "asio.hpp"

vector<out_chain> out_head;
vector<routing_chain> routing_head;

static void out_vector(ptree root);
static void routing_vector(ptree root);

int main(int argc, char const *argv[]) {
  ptree root;

  read_json("dns.json", root);

  out_vector(root.get_child("out", ptree{}));
  routing_vector(root.get_child("routing", ptree{}));
  do_listen(root.get_child("in", ptree{}));

  return 0;
}

static void out_vector(ptree root) {
  out_chain no; /* new out_chain */

  if (root.empty()) {
    root.push_back(make_pair("", ptree{}));
  }

  for (auto &out : root) {
    no.tag = out.second.get("tag", "");
    no.address = out.second.get("address", "8.8.8.8");
    no.port = out.second.get("port", 53);

    out_head.push_back(move(no));
  }
}

static void routing_vector(ptree root) {
  routing_chain nr; /* new routing_chain */

  if (root.empty()) {
    root.push_back(make_pair("", ptree{}));
  }

  for (auto &routing : root) {
    for (auto &in_tag : routing.second.get_child("in_tag", ptree{})) {
      nr.in_tag.push_back(in_tag.second.data());
    }
    for (auto &domain : routing.second.get_child("domain", ptree{})) {
      nr.domain.push_back(domain.second.data());
    }
    nr.out_tag = routing.second.get("out_tag", "");

    routing_head.push_back(move(nr));
  }
}
