
******************************************************************************************************************************************************

	boost::beast

		1. create tcp connection
			
			a. tcp::reslover ->																					// return endpoint																		
			
			b. boost::beast::websocket::stream<boost::asio::ip::tcp::socket> ->									// try connecting
			// or tcp::socket


		2. tls handshake

			a. boost::asio::ssl::context::tlsv12_client ->														// create ssl context

			b. boost::beast::websocket::stream<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>> ->		// initialize tls websocket

			c. boost::asio::connect(.next_layer().next_layer(), tcp::reslover) ->								// connect to endpoint

			d. .next_layer().handshake(ssl::stream_base::client) ->												// tls handshake


		3. websocket handshake

			a. .handshake(host, target) ->																		// websocket handshake


		4. message

			a. .write(boost::asio::buffer(std::string(""))) ->													// send message

			b. .read(boost::beast::flat_buffer) ->																// read message

		
		xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

		// seems that the library itself automatically disconnects.

		5. disconnect
			
			a. net::io_context.stop() ->																		// io_context stop	

			b. .consume(.size()) ->																				// message buffer consume
			
			c. .close(boost::beast::websocket::close_code::normal) ->											// websocket close
																												
			d. .next_layer().shutdown() ->																		// tls shutdown

			e. .next_layer().next_layer().close() ->															// tcp close

		xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
																												

******************************************************************************************************************************************************


******************************************************************************************************************************************************

	binance

		end point:

			wss://stream.binance.com:9443/ws/stream

				443: standard port, compatible with firewall environments
				9443: binance-recommended port for websocket

				ws: single stream
				stream: multi stream
				ws/stream: ws for test(both binance and user)
        
		request:

			{
				"method": "SUBSCRIBE",
				"params": "btcusdt@depth5@100ms"
				"id": 1
			}
			
		depth channels:

			depth: full, 100ms or 1000ms
			depth5: 5, 100ms or 1000ms
			depth10: 10, 100ms or 1000ms
			depth20: 20, 100ms or 1000ms

		push data:

			/443/ws: {"e":"depthUpdate","E":1234567890123,"s":"BTCUSDT","U":123456789012,"u":123456789012,"b":["12345.67890123","1.23456789"],"a":["12345.67890123","1.23456789"]}
			/443/stream: {"stream":"btcusdt@depth","data":{"e":"depthUpdate","E":1234567890123,"s":"BTCUSDT","U":123456789012,"u":123456789012,"b":12345.67890123,"a":12345.67890123}
			/443/ws/stream: {"e":"depthUpdate","E":1234567890123,"s":"BTCUSDT","U":123456789012,"u":123456789012,"b":["12345.67890123","1.23456789"], "a":["12345.67890123","1.23456789"]}
			/9443/ws: {"e":"depthUpdate","E":1234567890123,"s":"BTCUSDT","U":123456789012,"u":123456789012,"b":["12345.67890123","1.23456789"], "a":["12345.67890123","1.23456789"]}
			/9443/stream: {"stream":"btcusdt@depth","data":{"e":"depthUpdate","E":1234567890123,"s":"BTCUSDT","U":123456789012,"u":123456789012,"b":["12345.67890123","1.23456789"], "a":["12345.67890123","1.23456789"]}
			/9443/ws/stream: {"e":"depthUpdate","E":1234567890123,"s":"BTCUSDT","U":123456789012,"u":123456789012,"b":["12345.67890123","1.23456789"], "a":["12345.67890123","1.23456789"]}


******************************************************************************************************************************************************


******************************************************************************************************************************************************

	coinbase

		end point:
			
			wss://ws-feed.exchange.coinbase.com

		request:

			{
				"type": "subscribe",
				"channels": [
					{
						"name": "ticker", 
						"product_ids": "BTC-USD"
					}
				]
			}

		depth channels:

			ticker: 1, real-time
			level2: full, snapshot first, psuh increment for changes, real-time
			level2_batch_: full, snapshot first, psuh increment for changes, 50ms
		
		push data:
			
			// ticker
			{
				... ,
				"type": "ticker",
				... ,
				"side": "buy" //or "sell"
				"best_bid": "12345.67",
				"best_ask": "12345.67","
				, ...
			}
			
			// level2 //and level2_batch
			{
				... ,
				"type": "l2update",
				... ,
				"changes": [["side1","price1","size1"], ["side2","price2","size2"], ["side3","price3","size3"], ...]
				, ...
			}


******************************************************************************************************************************************************


******************************************************************************************************************************************************

	bybit
		
		end point: 
		
			wss://stream.bybit.com/v5/public/spot

		request:
		
			{
				"op": "subscribe",
				"args": "orderbook.1.BTCUSDT"
			}

		depth channels: 

			orderbook.1: 1, snapshot fist + psuh increment for changes, 10ms
			orderbook.50: 50, snapshot, 20ms
			orderbook.100: 100, snapshot, 100ms

		push data:
			
			{
				... ,
				"type": "snapshot" //or "delta"
				"data": {
					... ,
					"b": [
					["price1","quantity1"], 
					["price2","quantity2"], 
					],
					"a": [
					["price1","quantity1"], 
					["price2","quantity2"], 
					],
					, ...
				}
			}


******************************************************************************************************************************************************


******************************************************************************************************************************************************

	okx

		end point: 
		
			wss://ws.okx.com:8443/ws/v5/public

		request:

			{
				"op": "subscribe",
				"args": [
					{
						"channel": "books",
						"instId": "BTC-USDT"
					}
				]
			}

		depth channels: 

			books: 400, snapshot fist + push increment for changes, real-time
			books5: 5, snapshot, 100ms
			books50-12-tbt: 50, snapshot, tick-by-tick(real-time)
			books-12-tbt: full, tick-by-tick(real-time)

		push data:
			
			{
				... ,
				"action": "snapshot" //or "update"
				"data": [
					{
						"asks": [
							["price1", "quantity1", "accumulation1"],
							["price2", "quantity2", "accumulation2"],
					    ],
				        "bids": [
							["price1", "quantity1", "accumulation1"],
							["price2", "quantity2", "accumulation2"],
						],
					, ...
					}
				]
			}


******************************************************************************************************************************************************


******************************************************************************************************************************************************

	bitget

		end point: wss://ws.bitget.com/spot/v1/stream

		request:
			{
				"op": "subscribe",
				"args": [
					{
						"instType": "sp",
						"channel": "books"
						"instId": "BTCUSDT",
					}
				]
			}
		
		depth channels: 

			books: full, snapshot fist + push increment for changes, real-time
			books5: 5, snapshot, real-time
			books15: 15, snapshot,, real-time

		push data:
			
			{
				"action": "snapshot" //or "update"
				, ...
				"data": [
					{
						"asks": [
							["price1", "quantity1"],
							["price2", "quantity2"],
						],
						"bids": [
							["price1", "quantity1"],
							["price2", "quantity2"],
						],
						, ...
					}
				]
			}


******************************************************************************************************************************************************