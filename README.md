# Pico ADC Acquisition trough SPI Transmission

This firmware turns a Raspberry Pi Pico (RP2040) into a **streaming front-end** for a **16-bit parallel ADC (ADS8405)** and exposes the captured samples to an upstream host (here a Raspberry Pi) that **pulls data over SPI** (host = SPI **master**, Pico = SPI **slave**) in real time.

## Requirements

- Raspberry Pi Pico (RP2040)
- ADS8405 16-bit ADC
- CMake and Pico SDK
- Raspberry Pi (or equivalent) as SPI master to pull out data

## High-Level Overview

### Key properties

- Acquisition: RP2040 PIO latches the 16-bit parallel output of the ADS8405 in sync with the ADC timing (BUSY/CONVST/CLK).

- Transport: Samples are buffered in RAM and served over SPI with the Pico acting as an SPI slave while the host clocks data out as needed.

- PIO sampler: Minimal state machine that captures the 16-bits bus on the appropriate edge.

- DMA: Chained DMA moves fixed-size blocks from the PIO FIFO into a lock-free ring buffer in SRAM.

- Ring buffer: Single-producer (DMA) / single-consumer (SPI ISR) queue.

- SPI link: SPI slave TX path reads from the ring buffer.

- Control/IRQ: IRQ for DMA completion, PIO FIFO service, and SPI events.

### Host–Device Protocol
- Electrical: Standard single-duplex SPI (no MOSI)

- Payload: 1 little-endian 16-bit samples

- Flow control: Host is master and controls extraction rate. Device exposes buffer fill trough the READY line.

RP2040 comfortably handles the 20MHz SPI as a slave.

## Build & Integration
Project uses CMake and the Pico SDK (see repo root CMakeLists.txt and pico_sdk_import.cmake). 
