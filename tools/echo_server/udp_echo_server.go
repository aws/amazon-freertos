package main

import (
	"io"
	"log"
	"net"
)

//************** CONFIG SECTION *************//
// Port must be same as what test client is configured with (aws_test_tcp.h)
const sUdpEchoPort string = "8883"
const xReadTimeoutSeconds = 10

func main() {
	// listen on all interfaces
	xConnection, xStatus := net.ListenPacket( "udp", ":" + sUdpEchoPort )
	if xStatus != nil {
			log.Printf( "Error %s while trying to listen", xStatus )
	}
	log.Print( "UDP echo  server Listening to port " + sUdpEchoPort )

	echoServerThread( xConnection )
}


func echoServerThread(xConnection net.PacketConn) {
	defer xConnection.Close()

	xDataBuffer := make( []byte, 4096 )

	// Listen for packets from clients and echo the content back
	for {
			xNbBytes, xAddr, xStatus := xConnection.ReadFrom( xDataBuffer )
			if xStatus != nil {
					//EOF mean end of connection
					if xStatus != io.EOF {
							//If not EOF then it is an error
							log.Print( "UDP echo server ReadFrom failed status ", xStatus )
					}
					break
			}

			// xNbBytes, xStatus = xConnection.Write(xDataBuffer[:xNbBytes])
			xNbBytes, xStatus = xConnection.WriteTo( xDataBuffer[:xNbBytes], xAddr )
			if xStatus != nil {
					log.Printf( "Error %s while sending data", xStatus )
					break
			}
	}
}
