/*
 * libiio - AD9371 IIO streaming example
 *
 * Copyright (C) 2014 IABG mbH
 * Author: Michael Feilen <feilen_at_iabg.de>
 * Copyright (C) 2017 Analog Devices Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 **/

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <time.h>

#ifdef __APPLE__
#include <iio/iio.h>
#else
#include <iio.h>
#endif

/* helper macros */
#define MHZ(x) ((long long)(x*1000000.0 + .5))
#define GHZ(x) ((long long)(x*1000000000.0 + .5))

#define ASSERT(expr) { \
	if (!(expr)) { \
		(void) fprintf(stderr, "assertion failed (%s:%d)\n", __FILE__, __LINE__); \
		(void) abort(); \
	} \
}

/* RX is input, TX is output */
enum iodev { RX, TX };

/* common RX and TX streaming params */
struct stream_cfg {
	long long bw_hz; // Analog banwidth in Hz
	long long fs_hz; // Baseband sample rate in Hz
	long long lo_hz; // Local oscillator frequency in Hz
};

/* static scratch mem for strings */
static char tmpstr[64];

/* IIO structs required for streaming */
static struct iio_context *ctx   = NULL;

static struct iio_channel *rx0_i = NULL;
static struct iio_channel *rx0_q = NULL;
static struct iio_channel *rx1_i = NULL;
static struct iio_channel *rx1_q = NULL;

static struct iio_channel *tx0_i = NULL;
static struct iio_channel *tx0_q = NULL;
static struct iio_channel *tx1_i = NULL;
static struct iio_channel *tx1_q = NULL;

static struct iio_buffer  *rxbuf = NULL;
static struct iio_buffer  *txbuf = NULL;

static bool stop;

/* cleanup and exit */
static void shutdown()
{
	printf("* Destroying buffers\n");
	if (rxbuf) { iio_buffer_destroy(rxbuf); }
	if (txbuf) { iio_buffer_destroy(txbuf); }

	printf("* Disabling streaming channels\n");
	if (rx0_i) { iio_channel_disable(rx0_i); }
	if (rx0_q) { iio_channel_disable(rx0_q); }
	if (tx0_i) { iio_channel_disable(tx0_i); }
	if (tx0_q) { iio_channel_disable(tx0_q); }

	printf("* Destroying context\n");
	if (ctx) { iio_context_destroy(ctx); }
	exit(0);
}

static void handle_sig(int sig)
{
	printf("Waiting for process to finish...\n");
	stop = true;
}

/* check return value of attr_write function */
static void errchk(int v, const char* what) {
	 if (v < 0) { fprintf(stderr, "Error %d writing to channel \"%s\"\nvalue may not be supported.\n", v, what); shutdown(); }
}

/* write attribute: long long int */
static void wr_ch_lli(struct iio_channel *chn, const char* what, long long val)
{
	errchk(iio_channel_attr_write_longlong(chn, what, val), what);
}

/* write attribute: long long int */
static long long rd_ch_lli(struct iio_channel *chn, const char* what)
{
	long long val;

	errchk(iio_channel_attr_read_longlong(chn, what, &val), what);

	printf("\t %s: %lld\n", what, val);
	return val;
}

#if 0
/* write attribute: string */
static void wr_ch_str(struct iio_channel *chn, const char* what, const char* str)
{
	errchk(iio_channel_attr_write(chn, what, str), what);
}
#endif

/* helper function generating channel names */
static char* get_ch_name_mod(const char* type, int id, char modify)
{
	snprintf(tmpstr, sizeof(tmpstr), "%s%d_%c", type, id, modify);
	return tmpstr;
}

/* helper function generating channel names */
static char* get_ch_name(const char* type, int id)
{
	snprintf(tmpstr, sizeof(tmpstr), "%s%d", type, id);
	return tmpstr;
}

/* returns ad9371 phy device */
static struct iio_device* get_ad9371_phy(struct iio_context *ctx)
{
	struct iio_device *dev =  iio_context_find_device(ctx, "ad9371-phy");
	ASSERT(dev && "No ad9371-phy found");
	return dev;
}

/* finds AD9371 streaming IIO devices */
static bool get_ad9371_stream_dev(struct iio_context *ctx, enum iodev d, struct iio_device **dev)
{
	switch (d) {
	case TX: *dev = iio_context_find_device(ctx, "axi-ad9371-tx-hpc"); return *dev != NULL;
	case RX: *dev = iio_context_find_device(ctx, "axi-ad9371-rx-hpc");  return *dev != NULL;
	default: ASSERT(0); return false;
	}
}

/* finds AD9371 streaming IIO channels */
static bool get_ad9371_stream_ch(struct iio_context *ctx, enum iodev d, struct iio_device *dev, int chid, char modify, struct iio_channel **chn)
{
	*chn = iio_device_find_channel(dev, modify ? get_ch_name_mod("voltage", chid, modify) : get_ch_name("voltage", chid), d == TX);
	if (!*chn)
		*chn = iio_device_find_channel(dev, modify ? get_ch_name_mod("voltage", chid, modify) : get_ch_name("voltage", chid), d == TX);
	return *chn != NULL;
}

/* finds AD9371 phy IIO configuration channel with id chid */
static bool get_phy_chan(struct iio_context *ctx, enum iodev d, int chid, struct iio_channel **chn)
{
	switch (d) {
	case RX: *chn = iio_device_find_channel(get_ad9371_phy(ctx), get_ch_name("voltage", chid), false); return *chn != NULL;
	case TX: *chn = iio_device_find_channel(get_ad9371_phy(ctx), get_ch_name("voltage", chid), true);  return *chn != NULL;
	default: ASSERT(0); return false;
	}
}

/* finds AD9371 local oscillator IIO configuration channels */
static bool get_lo_chan(struct iio_context *ctx, enum iodev d, struct iio_channel **chn)
{
	switch (d) {
	 // LO chan is always output, i.e. true
	case RX: *chn = iio_device_find_channel(get_ad9371_phy(ctx), get_ch_name("altvoltage", 0), true); return *chn != NULL;
	case TX: *chn = iio_device_find_channel(get_ad9371_phy(ctx), get_ch_name("altvoltage", 1), true); return *chn != NULL;
	default: ASSERT(0); return false;
	}
}

/* applies streaming configuration through IIO */
bool cfg_ad9371_streaming_ch(struct iio_context *ctx, struct stream_cfg *cfg, enum iodev type, int chid)
{
	struct iio_channel *chn = NULL;

	// Configure phy and lo channels
	printf("* Acquiring AD9371 phy %s channel %d\n", type == TX ? "TX" : "RX", chid);
	if (!get_phy_chan(ctx, type, chid, &chn)) {	return false; }
	//wr_ch_lli(chn, "rf_bandwidth",       cfg->bw_hz);
	//wr_ch_lli(chn, "sampling_frequency", cfg->fs_hz);
	rd_ch_lli(chn, "rf_bandwidth");
	rd_ch_lli(chn, "sampling_frequency");

	// Configure LO channel
	printf("* Acquiring AD9371 %s lo channel\n", type == TX ? "TX" : "RX");
	if (!get_lo_chan(ctx, type, &chn)) { return false; }
	wr_ch_lli(chn, type == TX ? "TX_LO_frequency" : "RX_LO_frequency" , cfg->lo_hz);
	return true;
}

// NOTICE: > 1M-samples will get "Cannot allocate memory"
#ifdef ENABLE_PORT1
#define LIBIIO_PORT_SAMPLES (1024*1024/2)
#else
//#define LIBIIO_PORT_SAMPLES (1024*1024/1)
	// iio_buffer_foreach_sample() Throughput  140.470 Mbps
	//					  memcpy() Throughput 1364.750 Mbps

#define LIBIIO_PORT_SAMPLES (1024*1024/2)
	// iio_buffer_foreach_sample() Throughput  140.483 Mbps
	// 					  memcpy() Throughput 1365.490 Mbps

//#define LIBIIO_PORT_SAMPLES 1200
	// iio_buffer_foreach_sample() Throughput   10.451 Mbps
	//					  memcpy() Throughput   10.379 Mbps

#endif

uint8_t eth_txbuf[80+1200*4*1];
uint8_t eth_rxbuf[90+1200*4*1];

/* simple configuration and streaming */
int main (int argc, char **argv)
{
	struct timespec tm_xs, tm_xe;
	double tm_us;

	// Streaming devices
	struct iio_device *tx;
	struct iio_device *rx;

	// RX and TX sample counters
	size_t nrx = 0;
	size_t ntx = 0;

	// Stream configurations
	struct stream_cfg rxcfg;
	struct stream_cfg txcfg;

	// Listen to ctrl+c and ASSERT
	signal(SIGINT, handle_sig);

	// RX stream config
	rxcfg.lo_hz = GHZ(2.5); // 2.5 GHz rf frequency

	// TX stream config
	txcfg.lo_hz = GHZ(2.5); // 2.5 GHz rf frequency

	printf("* Acquiring IIO context\n");
	ASSERT((ctx = iio_create_default_context()) && "No context");
	ASSERT(iio_context_get_devices_count(ctx) > 0 && "No devices");

	printf("* Acquiring AD9371 streaming devices\n");
	ASSERT(get_ad9371_stream_dev(ctx, TX, &tx) && "No txdev found");
	ASSERT(get_ad9371_stream_dev(ctx, RX, &rx) && "No rxdev found");

	printf("* Configuring AD9371 for streaming\n");
	ASSERT(cfg_ad9371_streaming_ch(ctx, &rxcfg, RX, 0) && "RX port0 not found");
	ASSERT(cfg_ad9371_streaming_ch(ctx, &txcfg, TX, 0) && "TX port0 not found");
#ifdef ENABLE_PORT1
	ASSERT(cfg_ad9371_streaming_ch(ctx, &rxcfg, RX, 1) && "RX port1 not found");
	ASSERT(cfg_ad9371_streaming_ch(ctx, &txcfg, TX, 1) && "TX port1 not found");
#endif

	printf("* Initializing AD9371 IIO streaming channels\n");
	ASSERT(get_ad9371_stream_ch(ctx, RX, rx, 0, 'i', &rx0_i) && "RX port0chan_i not found");
	ASSERT(get_ad9371_stream_ch(ctx, RX, rx, 0, 'q', &rx0_q) && "RX port0chan_q not found");
	ASSERT(get_ad9371_stream_ch(ctx, TX, tx, 0, 0, &tx0_i) && "TX port0chan_i not found");
	ASSERT(get_ad9371_stream_ch(ctx, TX, tx, 1, 0, &tx0_q) && "TX port0chan_q not found");
#ifdef ENABLE_PORT1
	ASSERT(get_ad9371_stream_ch(ctx, RX, rx, 1, 'i', &rx1_i) && "RX port1chan_i not found");
	ASSERT(get_ad9371_stream_ch(ctx, RX, rx, 1, 'q', &rx1_q) && "RX port1chan_q not found");
	ASSERT(get_ad9371_stream_ch(ctx, TX, tx, 2, 0, &tx1_i) && "TX port1chan_i not found");
	ASSERT(get_ad9371_stream_ch(ctx, TX, tx, 3, 0, &tx1_q) && "TX port1chan_q not found");
#endif

	printf("* Enabling IIO streaming channels\n");
	iio_channel_enable(rx0_i);
	iio_channel_enable(rx0_q);
	iio_channel_enable(tx0_i);
	iio_channel_enable(tx0_q);
#ifdef ENABLE_PORT1
	iio_channel_enable(rx1_i);
	iio_channel_enable(rx1_q);
	iio_channel_enable(tx1_i);
	iio_channel_enable(tx1_q);
#endif

	printf("* Creating non-cyclic IIO buffers with %d Smp\n", LIBIIO_PORT_SAMPLES);
	rxbuf = iio_device_create_buffer(rx, LIBIIO_PORT_SAMPLES, false);
	if (!rxbuf) {
		perror("Could not create RX buffer");
		shutdown();
	}
	txbuf = iio_device_create_buffer(tx, LIBIIO_PORT_SAMPLES, false);
	if (!txbuf) {
		perror("Could not create TX buffer");
		shutdown();
	}

	printf("* Starting IO streaming (press CTRL+C to cancel)\n");
	
	clock_gettime(CLOCK_MONOTONIC, &tm_xs);
	while (!stop)
	{
		ssize_t nbytes_rx, nbytes_tx;
		char *p_dat, *p_end;
		ptrdiff_t p_inc;

		// Schedule TX buffer
		nbytes_tx = iio_buffer_push(txbuf);
		if (nbytes_tx < 0) { printf("Error pushing buf %d\n", (int) nbytes_tx); shutdown(); }

		// Refill RX buffer
		nbytes_rx = iio_buffer_refill(rxbuf);
		if (nbytes_rx < 0) { printf("Error refilling buf %d\n",(int) nbytes_rx); shutdown(); }

		// READ: Get pointers to RX buf and read IQ from RX buf port 0
		p_inc = iio_buffer_step(rxbuf);
		p_end = iio_buffer_end(rxbuf);
#if 0
		for (p_dat = iio_buffer_first(rxbuf, rx0_i); p_dat < p_end; p_dat += p_inc) {
			// Example: swap I and Q
			const int16_t p0i = ((int16_t*)p_dat)[0]; // Real (I)
			const int16_t p0q = ((int16_t*)p_dat)[1]; // Imag (Q)
			((int16_t*)p_dat)[0] = p0q;
			((int16_t*)p_dat)[1] = p0i;
		}
#else
		size_t n1rx = nbytes_rx;
		p_dat = iio_buffer_first(rxbuf, rx0_i);
		do {
			memcpy(eth_txbuf+80, p_dat, 1200*4);
			//sendto(fd, eth_txbuf+80, 1200*4);
			n1rx -= 1200*4;
			p_dat += 1200*4;
			if (n1rx <= 1200*4) {
				memcpy(eth_txbuf+80, p_dat, n1rx);
				//sendto(fd, eth_txbuf+80, 1200);
				break;
			}
		} while(1);
#endif
#ifdef ENABLE_PORT1
		for (p_dat = iio_buffer_first(rxbuf, rx1_i); p_dat < p_end; p_dat += p_inc) {
			// Example: swap I and Q
			const int16_t p1i = ((int16_t*)p_dat)[0]; // Real (I)
			const int16_t p1q = ((int16_t*)p_dat)[1]; // Imag (Q)
			((int16_t*)p_dat)[0] = p1q;
			((int16_t*)p_dat)[1] = p1i;
		}
#endif

		// WRITE: Get pointers to TX buf and write IQ to TX buf port 0
		p_inc = iio_buffer_step(txbuf);
		p_end = iio_buffer_end(txbuf);
#if 0
		for (p_dat = iio_buffer_first(txbuf, tx0_i); p_dat < p_end; p_dat += p_inc) {
			// Example: fill with zeros
			// 14-bit sample needs to be MSB alligned so shift by 2
			// https://wiki.analog.com/resources/eval/user-guides/ad-fmcomms2-ebz/software/basic_iq_datafiles#binary_format
			((int16_t*)p_dat)[0] = 0 << 2; // Real (I)
			((int16_t*)p_dat)[1] = 0 << 2; // Imag (Q)
		}
#else
		p_dat = iio_buffer_first(txbuf, tx0_i);
		size_t n1tx = p_end - p_dat;
		do {
			//recvfrom(fd, eth_rxbuf+90, 1200*4, 0/*MSG_DONTWAIT*/, NULL, NULL);
			memcpy(p_dat, eth_rxbuf+90, 1200*4);
			n1tx -= 1200*4;
			if (n1tx <= 1200*4) {
				//recvfrom(fd, eth_rxbuf+90, n1tx, 0/*MSG_DONTWAIT*/, NULL, NULL);
				memcpy(p_dat, eth_rxbuf+90, n1tx);
				break;
			}
		} while(1);
#endif
#ifdef ENABLE_PORT1
		for (p_dat = iio_buffer_first(txbuf, tx1_i); p_dat < p_end; p_dat += p_inc) {
			// Example: fill with zeros
			// 14-bit sample needs to be MSB alligned so shift by 2
			// https://wiki.analog.com/resources/eval/user-guides/ad-fmcomms2-ebz/software/basic_iq_datafiles#binary_format
			((int16_t*)p_dat)[0] = 0 << 2; // Real (I)
			((int16_t*)p_dat)[1] = 0 << 2; // Imag (Q)
		}
#endif
		// Sample counter increment and status output
		nrx += nbytes_rx / iio_device_get_sample_size(rx);
		ntx += nbytes_tx / iio_device_get_sample_size(tx);
		printf("\tRX[0] %8.2f MSmp, TX[0] %8.2f MSmp\n", nrx/1e6, ntx/1e6);
#ifdef ENABLE_PORT1
		printf("\tRX[1] %8.2f MSmp, TX[1] %8.2f MSmp\n", nrx/1e6, ntx/1e6);
#endif
	}
	clock_gettime(CLOCK_MONOTONIC, &tm_xe);
	tm_us = elapse_us(&tm_xe, &tm_xs);
	printf("Elaps %.0f us\n", tm_us);
	printf("RX[0] Throughput %7.3f Mbps, TX[0] Throughput %7.3f Mbps\n",
		nrx*iio_device_get_sample_size(rx)/tm_us*8,
		ntx*iio_device_get_sample_size(tx)/tm_us*8);
#ifdef ENABLE_PORT1
	printf("RX[1] Throughput %7.3f Mbps, TX[1] Throughput %7.3f Mbps\n",
		nrx*iio_device_get_sample_size(rx)/tm_us*8,
		ntx*iio_device_get_sample_size(tx)/tm_us*8);
#endif

	shutdown();

	return 0;
}