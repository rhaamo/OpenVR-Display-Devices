#pragma once

#include "config.h"

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/signals2.hpp>
#include <iostream>

namespace net = boost::asio;
namespace beast = boost::beast;
namespace websocket = beast::websocket;
using namespace std::chrono_literals;

struct Webchaussette {
	boost::signals2::signal<void(std::string)> callback;

	Webchaussette(std::string host, uint16_t port, std::string query)
		: _host(std::move(host))
		, _query(std::move(query))
		, _port(std::to_string(port)) {
	}

	void Start() {
		net::co_spawn(_ws.get_executor(), do_run(), net::detached);
	}
	void Stop() {
		do_stop().get();
	}
	void Send(std::string &msg) {
		if (!_ws.is_open()) {
			std::cout << "WebSocket not connected, do not send message" << std::endl;
			return;
		}
		std::cout << "WebSocket Send: " << msg << std::endl;
		_ws.write(net::buffer(msg));
	}

	~Webchaussette() {
		do_stop().wait(); // noexcept, please
	}

private:
	using tcp = net::ip::tcp;
	const std::string              _host, _port, _query;
	beast::flat_buffer             _buffer;
	net::thread_pool               _ioc{ 1 };
	websocket::stream<tcp::socket> _ws{ _ioc };
	bool                           _stopped{ false };

	net::awaitable<void> do_run() {
		for (; !_stopped; co_await async_sleep(50ms))
			try {
			co_await do_connect();
			for (;;)
				co_await do_read();
		} catch (boost::system::system_error const &se) {
			std::cerr << "Error: " << se.code().message() << std::endl;
		}
	}

	net::awaitable<void> async_sleep(auto duration) {
		co_await net::steady_timer(_ws.get_executor(), duration).async_wait(net::use_awaitable);
	}

	std::future<void> do_stop() {
		// we need to check if the websocket is running before trying to close() it...
		if (_ws.is_open()) {
			return dispatch(_ws.get_executor(), std::packaged_task<void()>([this] {
				_stopped = true;
				_ws.next_layer().cancel();
				_ws.close(websocket::normal);
																		   }));
		} else {
			std::cout << "Cannot stop a non-open websocket duh" << std::endl;
		}
	}

	net::awaitable<void> do_connect() {
		co_await async_connect(_ws.next_layer(), tcp::resolver(_ioc).resolve(_host, _port),
							   net::use_awaitable);

		_ws.set_option(websocket::stream_base::decorator([](websocket::request_type &req) {
			req.set(beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING " WsConnect");
														 }));

		co_await _ws.async_handshake(_host + ':' + _port, "/" + _query, net::use_awaitable);
		_buffer.clear();
	}

	net::awaitable<void> do_read() {
		do
			co_await _ws.async_read(_buffer, net::use_awaitable);
		while (!_ws.is_message_done());

		callback(beast::buffers_to_string(_buffer.cdata()));
		_buffer.clear();
	}
};
