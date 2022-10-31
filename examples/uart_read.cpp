//
// Created by vivi on 30.10.22.
//

#include <boost/asio.hpp>
#include <iostream>

using namespace std;
namespace ba = boost::asio;

#define BUFSIZE 256

int main() {
    ba::io_service io;
    // Open serial port
    ba::serial_port serial(io, "/dev/ttyACM0");
    // Configure basic serial port parameters: 115.2kBaud, 8N1
    serial.set_option(ba::serial_port_base::baud_rate(115200));
    serial.set_option(ba::serial_port_base::character_size(8 /* data bits */));
    serial.set_option(ba::serial_port_base::parity(ba::serial_port_base::parity::none));
    serial.set_option(ba::serial_port_base::stop_bits(ba::serial_port_base::stop_bits::one));
    // Read data in a loop and copy to stdout
    for (int i = 0; i < 3; i++) {
        char data[BUFSIZE];
        size_t n = serial.read_some(ba::buffer(data, BUFSIZE));
        // Write data to stdout
        std::cout.write(data, n);
    }
}