#ifndef MAIN_HPP
#define MAIN_HPP

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <iostream>
#include <memory>
#include <stdint.h>
#include <utility>
#include <vector>

using namespace std;
using namespace boost::property_tree;

struct out_chain {
  string tag;
  string address;
  uint16_t port;
};

struct routing_chain {
  vector<string> in_tag;
  vector<string> domain;
  string out_tag;
};

extern vector<out_chain> out_head;
extern vector<routing_chain> routing_head;

#endif /* MAIN_HPP */