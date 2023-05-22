#if (DEMO_RX_ESB_ACK_PL)

	// This is simple receiver with Enhanced ShockBurst:
	//   - RX address: 'ESB'
	//   - payload: 10 bytes
	//   - RF channel: CHANNEL_NUMBER (2400MHz + CHANNEL_NUMBER MHz)
	//   - data rate : nRF24_RATE
	//   - CRC scheme: 2 byte

    // Set RF channel
    nRF24_SetRFChannel(CHANNEL_NUMBER);

    // Set data rate
    nRF24_SetDataRate(nRF24_RATE);

    // Set CRC scheme
    nRF24_SetCRCScheme(nRF24_CRC_2byte);

    // Set address width, its common for all pipes (RX and TX)
    nRF24_SetAddrWidth(3);

    // Configure RX PIPE
    static const uint8_t nRF24_ADDR[] = {'E', 'S', 'B'};
    nRF24_SetAddr(nRF24_PIPE1, nRF24_ADDR); // program address for pipe
    nRF24_SetRXPipe(nRF24_PIPE1, nRF24_AA_ON, 10); // Auto-ACK: enabled, payload length: 10 bytes

    // Set TX power for Auto-ACK (maximum, to ensure that transmitter will hear ACK reply)
    nRF24_SetTXPower(nRF24_TXPWR_0dBm);

    // Set operational mode (PRX == receiver)
    nRF24_SetOperationalMode(nRF24_MODE_RX);

    // Clear any pending IRQ flags
    nRF24_ClearIRQFlags();

    // Wake the transceiver
    nRF24_SetPowerMode(nRF24_PWR_UP);

    // Enable DPL
    nRF24_SetDynamicPayloadLength(nRF24_DPL_ON);

    nRF24_SetPayloadWithAck(1);

        // Put the transceiver to the RX mode
    nRF24_CE_H();

    // The main loop
#define PAYLOAD_LENGTH 6

/*
Test the direct access to the video memory
*/
#define no__DEBUG__
#ifdef __DEBUG__
    while(1){
    	uint32_t Im_size = 480*272*2/4; // size in 4-byte words
    	for (uint32_t pos=0; pos<Im_size; pos++){
            // Simulate the reception of a 6-byte length packet
			uint32_t val = pos;
			nRF24_payload[0] = val & 0xff; val >>= 8;
			nRF24_payload[1] = val & 0xff;

            // first 16-bit RGB565 pixel
			nRF24_payload[2] = 0x1f; // blue
			nRF24_payload[3] = 0x00;

            // second 16-bit RGB565 pixel
			nRF24_payload[4] = 0x00; // red
			nRF24_payload[5] = 0xf8;

            uint32_t pos = (nRF24_payload[1]<<8) | nRF24_payload[0];

            uint32_t color1_RGB565   = (nRF24_payload[3] << 8) | nRF24_payload[2];
            uint32_t color2_RGB565   = (nRF24_payload[5] << 8) | nRF24_payload[4];
            // write directly in the video SDRAM memory
            *(__IO uint32_t*) (LCD_FRAME_BUFFER + 4*pos) =  (color2_RGB565<<16) | color1_RGB565;
            // write 2 pixels at the same time
    	}
    }
#endif

while (1) {
    //
    // Constantly poll the status of the RX FIFO and get a payload if FIFO is not empty
    //
    // This is far from best solution, but it's ok for testing purposes
    // More smart way is to use the IRQ pin :)
    //
    if (nRF24_GetStatus_RXFIFO() != nRF24_STATUS_RXFIFO_EMPTY) {
        // Get a payload from the transceiver
        pipe = nRF24_ReadPayloadDpl(nRF24_payload, &payload_length);
        if(payload_length > 0) {
            nRF24_WriteAckPayload(pipe, "aCk PaYlOaD",11);
        }

        // Clear all pending IRQ flags
        nRF24_ClearIRQFlags();

        // Print a payload contents to UART
        UART_SendStr("RCV PIPE#");
        UART_SendInt(pipe);
        UART_SendStr(" PAYLOAD:>");
        Toggle_LED();
        UART_SendBufHex((char *) nRF24_payload, payload_length);
        UART_SendStr("<\r\n");

        // Receive data packet
        uint32_t pos = (nRF24_payload[1]<<8) | nRF24_payload[0];

        uint32_t color1_RGB565   = (nRF24_payload[3] << 8) | nRF24_payload[2];
        uint32_t color2_RGB565   = (nRF24_payload[5] << 8) | nRF24_payload[4];
        *(__IO uint32_t*) (LCD_FRAME_BUFFER + 4*pos) =  (color2_RGB565<<16) | color1_RGB565;

    }//endif
}//endwhile

#endif // DEMO_RX_ESB_ACK_PL