#include <stdlib.h>

#include "libp2p/conn/dialer.h"
#include "test_helper.h"

int test_dialer_new() {
	char* peer_id = "QmQSDGgxSVTkHmtT25rTzQtc5C1Yg8SpGK3BTws8YsJ4x3";
	struct PrivateKey* private_key = libp2p_crypto_private_key_new();
	struct Dialer* dialer = libp2p_conn_dialer_new(peer_id, private_key);
	if (dialer == NULL)
		return 0;
	libp2p_conn_dialer_free(dialer);
	return 1;
}

int test_dialer_dial() {
	int retVal = 0;
	char* config_dir = "/home/parallels/.ipfs/config";
	char* destination_string = "/ip/192.210.179.217/tcp/4001";
	char* peer_id = NULL;
	struct PrivateKey* private_key = NULL;
	struct Dialer* dialer = NULL;
	struct MultiAddress* destination_address = NULL;
	struct Connection* conn = NULL;
	char* result = NULL;
	size_t result_size = 0;

	test_helper_get_id_from_config(config_dir, &private_key, &peer_id);
	if (private_key == NULL)
		goto exit;

	dialer = libp2p_conn_dialer_new(peer_id, private_key);
	if (dialer == NULL)
		goto exit;

	destination_address = multiaddress_new_from_string(destination_string);
	if (destination_address == NULL)
		goto exit;

	// now try to dial
	conn = libp2p_conn_dialer_get_connection(dialer, destination_address);
	if (conn == NULL)
		goto exit;

	if (conn->write(conn, "ping", 4) == 0)
		goto exit;

	if (conn->read(conn, &result, &result_size) == 0)
		goto exit;

	if (result_size < 4 || strncmp(result, "pong", 4) != 0)
		goto exit;

	// clean up resources
	retVal = 1;
	exit:
	if (result != NULL)
		free(result);
	multiaddress_free(destination_address);
	libp2p_conn_dialer_free(dialer);
	libp2p_crypto_private_key_free(private_key);
	libp2p_conn_connection_free(conn);
	return retVal;
}
