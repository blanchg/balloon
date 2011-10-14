require 'socket'               # Get sockets from stdlib



trap("INT") { Kernel.exit }


def local_ip
  orig, Socket.do_not_reverse_lookup = Socket.do_not_reverse_lookup, true  # turn off reverse DNS resolution temporarily

  UDPSocket.open do |s|
    s.connect '64.233.187.99', 1
    s.addr.last
  end
ensure
  Socket.do_not_reverse_lookup = orig
end

port = 555
server = TCPServer.open(port)  # Socket to listen on port 2000
puts "Listening on port #{port} from #{local_ip}"
#log = File.open('log.txt', 'a');
loop {                         # Servers run forever
  client = server.accept       # Wait for a client to connect
  puts "Client connected"
  #log.puts "Client connected"
  while (result = Kernel.select([client], nil, nil, 5))
    break if result.empty?
    line = client.gets();
    break if line == nil
    line = line.chomp
    puts line
    #log.puts line
  end
  puts "Client disconnected"
  client.close                 # Disconnect from the client
}

