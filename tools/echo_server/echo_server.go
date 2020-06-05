/*
 * FreeRTOS Echo Server V2.0.0
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

package main

import (
	"crypto/rand"
	"crypto/tls"
	"crypto/x509"
	"encoding/json"
	"flag"
	"io"
	"io/ioutil"
	"log"
	"net"
	"os"
	"time"
)

const readTimeoutSecond = 300

// Argument struct for JSON configuration
type Argument struct {
	Verbose    bool   `json:"verbose"`
	Logging    bool   `json:"logging"`
	Secure     bool   `json:"secure-connection"`
	ServerPort string `json:"server-port"`
	ServerCert string `json:"server-certificate-location"`
	ServerKey  string `json:"server-key-location"`
}

func secureEcho(certPath string, keyPath string, port string, verbose bool) {

	// load certificates
	servertCert, err := tls.LoadX509KeyPair(certPath, keyPath)
	if err != nil {
		log.Fatalf("Error %s while loading server certificates", err)
	}

	serverCA, err := ioutil.ReadFile(certPath)
	if err != nil {
		log.Fatalf("Error %s while reading server certificates", err)
	}

	serverCAPool := x509.NewCertPool()
	serverCAPool.AppendCertsFromPEM(serverCA)

	//Configure TLS
	tlsConfig := tls.Config{Certificates: []tls.Certificate{servertCert},
		MinVersion: tls.VersionTLS12,
		RootCAs:    serverCAPool,
		ClientAuth: tls.RequireAnyClientCert,
		// Cipher suites supported by AWS IoT Core. Note this is the intersection of the set
		// of cipher suites supported by GO and by AWS IoT Core.
		// See the complete list of supported cipher suites at https://docs.aws.amazon.com/iot/latest/developerguide/transport-security.html.
		CipherSuites: []uint16{
			tls.TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256,
			tls.TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256,
			tls.TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384,
			tls.TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384,
			tls.TLS_AES_128_GCM_SHA256,
			tls.TLS_AES_256_GCM_SHA384,
		},
	}

	tlsConfig.Rand = rand.Reader
	echoServerThread(port, &tlsConfig, verbose)
}

func echoServerThread(port string, tlsConfig *tls.Config, verbose bool) {
	// listen on all interfaces
	var echoServer net.Listener
	var err error
	if tlsConfig != nil {
		echoServer, err = tls.Listen("tcp", ":"+port, tlsConfig)
		log.Println("Opening secure TCP server listening to port " + port)
	} else {
		echoServer, err = net.Listen("tcp", ":"+port)
		log.Println("Opening unsecure TCP server listening to port " + port)
	}

	if err != nil {
		log.Fatalf("While trying to listen for a connection an error occurred %s.", err)
	} else {
		defer echoServer.Close()
	}

	for {
		connection, err := echoServer.Accept()

		if err != nil {
			log.Printf("Error %s while trying to connect.", err)
		} else {
			go readWrite(connection, verbose)
		}
	}
}

func readWrite(connection net.Conn, verbose bool) {
	defer connection.Close()
	buffer := make([]byte, 4096)
	for {
		connection.SetReadDeadline(time.Now().Add(readTimeoutSecond * time.Second))
		readBytes, err := connection.Read(buffer)
		if err != nil {
			if err != io.EOF {
				log.Printf("Error %s while reading data. Expected an EOF to signal end of connection", err)
			}
			break
		} else {
			log.Printf("Read %d bytes.", readBytes)
			if verbose {
				log.Printf("Message:\n %s", buffer)
			}
		}
		writeBytes, err := connection.Write(buffer[:readBytes])
		if err != nil {
			log.Printf("Failed to send data with error: %s ", err)
			break
		}

		if writeBytes != 0 {
			log.Printf("Succesfully echoed back %d bytes.", writeBytes)
		}
	}
}

func startup(config Argument) {
	log.Println("Starting TCP Echo application...")
	if config.Secure {
		secureEcho(config.ServerCert, config.ServerKey, config.ServerPort, config.Verbose)
	}
	echoServerThread(config.ServerPort, nil, config.Verbose)
}

func logSetup() {
	echoLogFile, e := os.OpenFile("echo_server.log", os.O_CREATE|os.O_WRONLY|os.O_APPEND, 0666)
	if e != nil {
		log.Fatal("Failed to open log file.")
	}
	defer echoLogFile.Close()

	multi := io.MultiWriter(echoLogFile, os.Stdout)
	log.SetOutput(multi)
}

func main() {

	configLocation := flag.String("config", "./config.json", "Path to a JSON configuration.")
	flag.Parse()
	jsonFile, err := os.Open(*configLocation)

	if err != nil {
		log.Fatalf("Failed to open file with error: %s", err)
	}
	defer jsonFile.Close()

	byteValue, _ := ioutil.ReadAll(jsonFile)

	var config Argument
	err = json.Unmarshal(byteValue, &config)
	if err != nil {
		log.Fatalf("Failed to unmarshal json with error: %s", err)
	}

	if config.Logging {
		logSetup()
	}

	startup(config)
}
