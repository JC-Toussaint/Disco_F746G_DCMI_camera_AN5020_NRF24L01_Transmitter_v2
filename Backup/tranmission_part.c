#if (DEMO_TX_ESB_ACK_PL)

	// This is simple transmitter with Enhanced ShockBurst (to one logic address):
	//   - TX address: 'ESB'
	//   - payload: 6 bytes
	//   - RF channel: CHANNEL_NUMBER (2400MHz + CHANNEL_NUMBER MHz)
	//   - data rate : nRF24_RATE
	//   - CRC scheme: 2 byte

    // The transmitter sends a 6-byte packets to the address 'ESB' with Auto-ACK (ShockBurst enabled)

    // Set RF channel
    nRF24_SetRFChannel(CHANNEL_NUMBER);

    // Set data rate
    nRF24_SetDataRate(nRF24_RATE);

    // Set CRC scheme
    nRF24_SetCRCScheme(nRF24_CRC_2byte);

    // Set address width, its common for all pipes (RX and TX)
    nRF24_SetAddrWidth(3);

    // Configure TX PIPE
    static const uint8_t nRF24_ADDR[] = { 'E', 'S', 'B' };
    nRF24_SetAddr(nRF24_PIPETX, nRF24_ADDR); // program TX address
    nRF24_SetAddr(nRF24_PIPE0,  nRF24_ADDR); // program address for pipe#0, must be same as TX (for Auto-ACK)

    // Set TX power (maximum)
    nRF24_SetTXPower(nRF24_TXPWR_0dBm);

    // Configure auto retransmit: 10 retransmissions with pause of 2500s in between
    nRF24_SetAutoRetr(nRF24_ARD_2500us, 10);

    // Enable Auto-ACK for pipe#0 (for ACK packets)
    nRF24_EnableAA(nRF24_PIPE0);

    // Set operational mode (PTX == transmitter)
    nRF24_SetOperationalMode(nRF24_MODE_TX);

    // Clear any pending IRQ flags
    nRF24_ClearIRQFlags();

    // Enable DPL
    nRF24_SetDynamicPayloadLength(nRF24_DPL_ON);
	nRF24_SetPayloadWithAck(1);

	// Wake the transceiver
    nRF24_SetPowerMode(nRF24_PWR_UP);

    // Some variables
    uint32_t packets_lost = 0; // global counter of lost packets
    uint8_t otx;
    uint8_t otx_plos_cnt; // lost packet count
	uint8_t otx_arc_cnt; // retransmit count

//#define  RK043FN48H_WIDTH    ((uint16_t)480)          /* LCD PIXEL WIDTH            */
//#define  RK043FN48H_HEIGHT   ((uint16_t)272)          /* LCD PIXEL HEIGHT           */

	uint32_t Im_size_RGB565 = RK043FN48H_WIDTH * RK043FN48H_HEIGHT *2/4; // size in 4-byte words

// The main loop
while (1) {

#define PAYLOAD_LENGTH 6
    for (uint32_t pos=0; pos<Im_size_RGB565; pos++){

/*
Test the direct access to the video memory
*/
#define no__DEBUG__
#ifdef __DEBUG__
        *(__IO uint32_t*)(4*pos+LCD_FRAME_BUFFER) = 0xff00ff00;  // write 2 pixels at the same time
        uint32_t val =*(__IO uint32_t*)(4*pos+LCD_FRAME_BUFFER);
#endif
        // Prepare data packet
        payload_length = PAYLOAD_LENGTH; // MANDATORY assigned at each packet sending
        uint32_t val = pos;
        nRF24_payload[0] = val & 0xff; val >>= 8;
        nRF24_payload[1] = val & 0xff;

        val =*(__IO uint32_t*)(4*pos+LCD_FRAME_BUFFER);

        for (uint32_t i = 2; i < payload_length; i++) {
            nRF24_payload[i] = val & 0xff;  val >>= 8;
        }

        // Print a payload
        UART_SendStr("PAYLOAD:>");
        UART_SendBufHex((char *)nRF24_payload, payload_length);
        UART_SendStr("< ... TX: ");

        // Transmit a packet
        tx_res = nRF24_TransmitPacket(nRF24_payload, payload_length);
        otx = nRF24_GetRetransmitCounters();
        nRF24_ReadPayloadDpl(nRF24_payload, &payload_length );
        otx_plos_cnt = (otx & nRF24_MASK_PLOS_CNT) >> 4; // packets lost counter
        otx_arc_cnt  = (otx & nRF24_MASK_ARC_CNT); // auto retransmissions counter
        switch (tx_res) {
            case nRF24_TX_SUCCESS:
                UART_SendStr("OK");
                break;
            case nRF24_TX_TIMEOUT:
                UART_SendStr("TIMEOUT");
                break;
            case nRF24_TX_MAXRT:
                UART_SendStr("MAX RETRANSMIT");
                packets_lost += otx_plos_cnt;
                nRF24_ResetPLOS();
                break;
            default:
                UART_SendStr("ERROR");
                break;
        }
        UART_SendStr("   ACK_PAYLOAD=>");
        UART_SendBufHex((char *) nRF24_payload, payload_length);
        UART_SendStr("<   ARC=");
        UART_SendInt(otx_arc_cnt);
        UART_SendStr(" LOST=");
        UART_SendInt(packets_lost);
        UART_SendStr("\r\n");

        // Wait 10ms
        Delay_ms(10);
        Toggle_LED();

    }// endfor pos

}// endwhile


#endif // DEMO_TX_ESB_ACK_PL