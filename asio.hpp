#ifndef ASIO_HPP
#define ASIO_HPP

#include "main.hpp"
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <regex>
#include <stdio.h>

using namespace boost::asio;

void do_listen(ptree root);

#endif /* ASIO_HPP */