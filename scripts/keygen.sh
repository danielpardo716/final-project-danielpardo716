#!/bin/bash
# Script to generate certificates and keys for TLS

WORK_DIR=certs
SERVER_DIR=/etc/mosquitto/certs
CLIENT_DIR=base_external/rootfs_overlay/etc/mosquitto/certs
CA_SUBJECT="/C=US/ST=NY/L=Buffalo/O=AESD/CN=AESD"
CLIENT_SUBJECT="/C=US/ST=NY/L=Buffalo/O=AESD/CN=client"

mkdir -p "$WORK_DIR"
sudo cp configs/mosquitto-server.conf /etc/mosquitto/mosquitto.conf
killall mosquitto

# Create Certificate Authority (CA)
openssl genrsa -out "$WORK_DIR"/ca.key 4096
openssl req -x509 -new -nodes -key "$WORK_DIR"/ca.key -sha256 -days 3650 -out "$WORK_DIR"/ca.crt -subj "$CA_SUBJECT"

# Create Server certificate
#   1. Generate server private key.
#   2. Generate Certificate Signing Request (CSR) using the config file.
#   3. Sign server certificate with CA.
openssl genrsa -out "$WORK_DIR"/server.key 2048
openssl req -new -key "$WORK_DIR"/server.key -out "$WORK_DIR"/server.csr -config configs/openssl.conf
openssl x509 -req -in "$WORK_DIR"/server.csr -CA "$WORK_DIR"/ca.crt -CAkey "$WORK_DIR"/ca.key \
    -CAcreateserial -out "$WORK_DIR"/server.crt -days 365 -sha256 -extensions v3_req -extfile configs/openssl.conf

# Create Client certificate
#   1. Generate client private key.
#   2. Generate CSR.
#   3. Sign client certificate with CA.
openssl genrsa -out "$WORK_DIR"/client.key 2048
openssl req -new -key "$WORK_DIR"/client.key -out "$WORK_DIR"/client.csr -subj "$CLIENT_SUBJECT"
openssl x509 -req -in "$WORK_DIR"/client.csr -CA "$WORK_DIR"/ca.crt -CAkey "$WORK_DIR"/ca.key \
    -CAcreateserial -out "$WORK_DIR"/client.crt -days 365 -sha256

# Verify Server Cert
if ! openssl verify -CAfile "$WORK_DIR"/ca.crt "$WORK_DIR"/server.crt; then
    echo "ERROR: Server certificate verification failed!"
    exit 1
fi

# Verify Client Cert
if ! openssl verify -CAfile "$WORK_DIR"/ca.crt "$WORK_DIR"/client.crt; then
    echo "ERROR: Client certificate verification failed!"
    exit 1
fi

# Set permissions
sudo chmod 644 "$WORK_DIR"/ca.crt
sudo chmod 644 "$WORK_DIR"/server.crt
sudo chmod 600 "$WORK_DIR"/server.key
sudo chmod 644 "$WORK_DIR"/client.crt
sudo chmod 600 "$WORK_DIR"/client.key

# Move files to their proper locations
mkdir -p "$SERVER_DIR"
sudo cp "$WORK_DIR"/ca.key "$SERVER_DIR"/ca.key
sudo cp "$WORK_DIR"/ca.crt "$SERVER_DIR"/ca.crt
sudo cp "$WORK_DIR"/server.key "$SERVER_DIR"/server.key
sudo cp "$WORK_DIR"/server.crt "$SERVER_DIR"/server.crt

mkdir -p "$CLIENT_DIR"
sudo cp "$WORK_DIR"/ca.key "$CLIENT_DIR"/ca.key
sudo cp "$WORK_DIR"/ca.crt "$CLIENT_DIR"/ca.crt
sudo cp "$WORK_DIR"/client.key "$CLIENT_DIR"/client.key
sudo cp "$WORK_DIR"/client.crt "$CLIENT_DIR"/client.crt

# Verify files exist in destination
if [ ! -f "$SERVER_DIR/ca.crt" ] || [ ! -f "$CLIENT_DIR/ca.crt" ]; then
    echo "ERROR: Failed to copy CA files!"
    exit 1
fi

# Verify fingerprints match
SERVER_CA_FP=$(openssl x509 -in "$SERVER_DIR/ca.crt" -noout -fingerprint -sha256 | cut -d= -f2)
CLIENT_CA_FP=$(openssl x509 -in "$CLIENT_DIR/ca.crt" -noout -fingerprint -sha256 | cut -d= -f2)

if [ "$SERVER_CA_FP" != "$CLIENT_CA_FP" ]; then
    echo "ERROR: CA fingerprints do not match between Server and Client!"
    echo "Server: $SERVER_CA_FP"
    echo "Client: $CLIENT_CA_FP"
    exit 1
fi

# Restart broker
mosquitto -c /etc/mosquitto/mosquitto.conf -d